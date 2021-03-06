//
// Created by Mario Marchand on 16-12-30.
//

#include "ReseauGTFS.h"
#include <sys/time.h>

using namespace std;

//détermine le temps d'exécution (en microseconde) entre tv2 et tv2
long tempsExecution(const timeval &tv1, const timeval &tv2) {
    const long unMillion = 1000000;
    long dt_usec = tv2.tv_usec - tv1.tv_usec;
    long dt_sec = tv2.tv_sec - tv1.tv_sec;
    long dtms = unMillion * dt_sec + dt_usec;
    if (dtms < 0) throw logic_error("ReaseauGTFS::tempsExecution(): dtms doit être non négatif");
    return dtms;
}

size_t ReseauGTFS::getNbArcsOrigineVersStations() const {
    return m_nbArcsOrigineVersStations;
}

size_t ReseauGTFS::getNbArcsStationsVersDestination() const {
    return m_nbArcsStationsVersDestination;
}

double ReseauGTFS::getDistMaxMarche() const {
    return distanceMaxMarche;
}

//! \brief construit le réseau GTFS à partir des données GTFS
//! \param[in] Un objet DonneesGTFS
//! \post constuit un réseau GTFS représenté par un graphe orienté pondéré avec poids non négatifs
//! \post initialise la variable m_origine_dest_ajoute à false car les points origine et destination ne font pas parti du graphe
//! \post insère les données requises dans m_arretDuSommet et m_sommetDeArret et construit le graphe m_leGraphe
ReseauGTFS::ReseauGTFS(const DonneesGTFS &p_gtfs)
        : m_leGraphe(p_gtfs.getNbArrets()), m_origine_dest_ajoute(false) {

    //Le graphe possède p_gtfs.getNbArrets() sommets, mais il n'a pas encore d'arcs
    ajouterArcsVoyages(p_gtfs);
    ajouterArcsAttentes(p_gtfs);
    ajouterArcsTransferts(p_gtfs);
}

//! \brief ajout des arcs dus aux voyages
//! \brief insère les arrêts (associés aux sommets) dans m_arretDuSommet et m_sommetDeArret
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsVoyages(const DonneesGTFS &p_gtfs) {
    if (m_arretDuSommet.size() != 0){
        throw logic_error("Le graphe est déjà initialisé avec des noeuds, l'ajout supplémentaire est impossible");
    }

    const map<std::string, Voyage> &m_voyages = p_gtfs.getVoyages();
    size_t sommetCourant = m_arretDuSommet.size();

    for (auto itr = m_voyages.begin(); itr != m_voyages.end(); ++itr) {
        const set<Arret::Ptr, Voyage::compArret> m_arretsVoyage = itr->second.getArrets();

        auto voyage = m_arretsVoyage.begin();
        auto precedent = *voyage;

        // On insère les premiers arrets de chaque voyage
        m_sommetDeArret.insert({*voyage, sommetCourant});
        m_arretDuSommet.push_back(*voyage);

        ++sommetCourant;
        ++voyage;

        // La boucle itère sur un couple de valeurs et ajoute les arcs respectifs
        while (voyage != m_arretsVoyage.end()) {
            m_sommetDeArret.insert({*voyage, sommetCourant});
            m_arretDuSommet.push_back(*voyage);

            auto poids = (*voyage)->getHeureArrivee() - precedent->getHeureArrivee();

            if (poids < 0) {
                throw logic_error("Un poids négatif a été détecté");
            }

            m_leGraphe.ajouterArc((sommetCourant - 1), sommetCourant, poids);

            precedent = *voyage;
            ++sommetCourant;
            ++voyage;
        }
    }
}

//! \brief ajout des arcs dus aux attentes à chaque station
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsAttentes(const DonneesGTFS &p_gtfs) {
    const map<unsigned int, Station> &m_stations = p_gtfs.getStations();

    // On itère sur les stations
    for (auto station = m_stations.begin(); station != m_stations.end(); ++station) {
        const multimap<Heure, Arret::Ptr> &arretsStation = station->second.getArrets();

        // On note le premier arret d'une station
        auto arretStation = arretsStation.begin();
        auto precedent = arretStation;

        ++arretStation;
        while (arretStation != arretsStation.end()) {
            if ((*precedent).second->getVoyageId() != (*arretStation).second->getVoyageId()) {
                auto tempsAttente = (*arretStation).first - (*precedent).first;

                if (tempsAttente < 0){
                    throw logic_error("Une attente négative est impossible");
                }
                m_leGraphe.ajouterArc(m_sommetDeArret[(*precedent).second], m_sommetDeArret[(*arretStation).second],
                                      tempsAttente);
            }
            // On passe au prochain couple d'arrets
            precedent = arretStation;
            ++arretStation;
        }
    }
}


//! \brief ajouts des arcs dus aux transferts entre stations
//! \throws logic_error si une incohérence est détecté lors de cette étape de construction du graphe
void ReseauGTFS::ajouterArcsTransferts(const DonneesGTFS &p_gtfs) {
    const std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > &m_transferts = p_gtfs.getTransferts();
    const map<unsigned int, Station> &m_stations = p_gtfs.getStations();

    // On itère sur les tuples de transferts
    for (auto transfert = m_transferts.begin(); transfert != m_transferts.end(); ++transfert) {
        auto fromStationID = get<0>(*transfert);
        auto toStationID = get<1>(*transfert);
        auto transferTime = get<2>(*transfert);

        auto arretsSource = m_stations.find(fromStationID)->second.getArrets();
        auto arretsSuivants = m_stations.find(toStationID)->second.getArrets();

        for (auto arret = arretsSource.begin(); arret != arretsSource.end(); ++arret) {
            Heure heureArret = (*arret).first;
            auto prochainArret = arretsSuivants.lower_bound(heureArret.add_secondes(transferTime));

            if (prochainArret != arretsSuivants.end()) {
                auto tempsTransferts =  (*prochainArret).first - (*arret).first;

                if (tempsTransferts <= 0){
                    throw logic_error("Un transfert de 0 ou négatif a été détecté");
                }

                m_leGraphe.ajouterArc(m_sommetDeArret[(*arret).second], m_sommetDeArret[(*prochainArret).second],
                                      tempsTransferts);
            }
        }
    }
}

//! \brief ajoute des arcs au réseau GTFS à partir des données GTFS
//! \brief Il s'agit des arcs allant du point origine vers une station si celle-ci est accessible à pieds et des arcs allant d'une station vers le point destination
//! \param[in] p_gtfs: un objet DonneesGTFS
//! \param[in] p_pointOrigine: les coordonnées GPS du point origine
//! \param[in] p_pointDestination: les coordonnées GPS du point destination
//! \throws logic_error si une incohérence est détecté lors de la construction du graphe
//! \post constuit un réseau GTFS représenté par un graphe orienté pondéré avec poids non négatifs
//! \post assigne la variable m_origine_dest_ajoute à true (car les points orignine et destination font parti du graphe)
//! \post insère dans m_sommetsVersDestination les numéros de sommets connectés au point destination
void ReseauGTFS::ajouterArcsOrigineDestination(const DonneesGTFS &p_gtfs, const Coordonnees &p_pointOrigine,
                                               const Coordonnees &p_pointDestination) {
    if (m_origine_dest_ajoute == true){
        throw logic_error("Des arcs d'origine sont déjà présents dans le graphe");
    }

    const Heure heureDepart = p_gtfs.getTempsDebut();
    m_nbArcsStationsVersDestination = 0;
    m_nbArcsOrigineVersStations = 0;

    // On crée des Arrets pointeurs pour l'origine et la destination
    Arret::Ptr pointOrigine = make_shared<Arret>(stationIdOrigine, heureDepart, Heure(2, 0, 0), 0, "ORIGINE");
    Arret::Ptr pointDestination = make_shared<Arret>(stationIdDestination, heureDepart, Heure(2, 0, 0), 0,
                                                     "DESTINATION");

    m_leGraphe.resize(m_leGraphe.getNbSommets() + 2);

    m_sommetOrigine = m_leGraphe.getNbSommets() - 2;
    m_sommetDestination = m_leGraphe.getNbSommets() - 1;

    // Ajouts des sommets dans le graphe
    m_arretDuSommet.push_back(pointOrigine);
    m_arretDuSommet.push_back(pointDestination);

    m_sommetDeArret.insert({pointOrigine, m_sommetOrigine});
    m_sommetDeArret.insert({pointDestination, m_sommetDestination});


    const map<unsigned int, Station> &m_stations = p_gtfs.getStations();

    for (auto station = m_stations.begin(); station != m_stations.end(); ++station) {
        const Coordonnees &coordStation = station->second.getCoords();

        double distanceMarcheOrigine = p_pointOrigine - coordStation;
        double distanceMarcheDestination = p_pointDestination - coordStation;

        if (distanceMarcheOrigine < distanceMaxMarche) {
            const multimap<Heure, Arret::Ptr> &arretsStation = station->second.getArrets();

            unsigned int secondesMarche = ((distanceMarcheOrigine / vitesseDeMarche) * 3600);
            Heure tempsMarcheOrigine = heureDepart.add_secondes(secondesMarche);

            multimap<Heure, Arret::Ptr>::const_iterator arretAccessible = arretsStation.lower_bound(tempsMarcheOrigine);

            if (arretAccessible != arretsStation.end()) {
                m_leGraphe.ajouterArc(m_sommetOrigine, m_sommetDeArret[(*arretAccessible).second],
                                      (*arretAccessible).first - heureDepart);
                ++m_nbArcsOrigineVersStations;
            }
        }
        if (distanceMarcheDestination < distanceMaxMarche) {
            const multimap<Heure, Arret::Ptr> &arretsStation = station->second.getArrets();

            unsigned int tempsMarcheDestination = (distanceMarcheDestination / vitesseDeMarche) * 3600;

            for (auto arret = arretsStation.begin(); arret != arretsStation.end(); ++arret) {
                size_t sommetArret = m_sommetDeArret[(*arret).second];

                m_leGraphe.ajouterArc(sommetArret, m_sommetDestination, tempsMarcheDestination);
                m_sommetsVersDestination.push_back(sommetArret);
                ++m_nbArcsStationsVersDestination;
            }
        }
    }

    if (m_nbArcsStationsVersDestination == 0 or m_nbArcsOrigineVersStations == 0){
        throw logic_error("Aucun arrêt de bus n'est dans le rayon maximal de marche de la destination ou de l'origine");
    }

    // On indique que que les arcs origine/destination ont été ajoutés
    m_origine_dest_ajoute = true;
}

//! \brief Remet ReseauGTFS dans l'était qu'il était avant l'exécution de ReseauGTFS::ajouterArcsOrigineDestination()
//! \param[in] p_gtfs: un objet DonneesGTFS
//! \throws logic_error si une incohérence est détecté lors de la modification du graphe
//! \post Enlève de ReaseauGTFS tous les arcs allant du point source vers un arrêt de station et ceux allant d'un arrêt de station vers la destination
//! \post assigne la variable m_origine_dest_ajoute à false (les points orignine et destination sont enlevés du graphe)
//! \post enlève les données de m_sommetsVersDestination
void ReseauGTFS::enleverArcsOrigineDestination() {
    if (m_origine_dest_ajoute == false) {
        throw logic_error("Il n'y a pas d'arcs d'origine et de destination dans le graphe");
    }

    for (auto sommet = m_sommetsVersDestination.begin(); sommet != m_sommetsVersDestination.end(); ++sommet) {
        m_leGraphe.enleverArc(*sommet, m_sommetDestination);
    }
    m_leGraphe.resize(m_leGraphe.getNbSommets() - 2);

    // Allons chercher les pointeurs des sommets Origine et Destination
    Arret::Ptr sommetOrigine = m_arretDuSommet[m_sommetOrigine];
    Arret::Ptr sommetDestination = m_arretDuSommet[m_sommetDestination];

    // Suppression m_sommetDeArret
    m_sommetDeArret.erase(sommetOrigine);
    m_sommetDeArret.erase(sommetDestination);

    // Suppression des sommets dans m_arretDuSommet
    m_arretDuSommet.pop_back();
    m_arretDuSommet.pop_back();

    // Mise à jour des paramètres du graphe
    m_nbArcsOrigineVersStations = 0;
    m_nbArcsStationsVersDestination = 0;
    m_sommetsVersDestination.clear();
    m_origine_dest_ajoute = false;
}


//! \brief Trouve le plus court chemin menant du point d'origine au point destination préalablement choisis
//! \brief Permet également d'affichier l'itinéraire du voyage et retourne le temps d'exécution de l'algorithme de plus court chemin utilisé
//! \param[in] p_afficherItineraire: true si on désire afficher l'itinéraire et false autrement
//! \param[out] p_tempsExecution: le temps d'exécution de l'algorithme de plus court chemin utilisé
//! \throws logic_error si un problème survient durant l'exécution de la méthode
void ReseauGTFS::itineraire(const DonneesGTFS &p_gtfs, bool p_afficherItineraire, long &p_tempsExecution) const {
    if (!m_origine_dest_ajoute)
        throw logic_error(
                "ReseauGTFS::afficherItineraire(): il faut ajouter un point origine et un point destination avant d'obtenir un itinéraire");

    vector<size_t> chemin;

    timeval tv1;
    timeval tv2;
    if (gettimeofday(&tv1, 0) != 0)
        throw logic_error("ReseauGTFS::afficherItineraire(): gettimeofday() a échoué pour tv1");
    unsigned int tempsDuTrajet = m_leGraphe.plusCourtChemin(m_sommetOrigine, m_sommetDestination, chemin);
    if (gettimeofday(&tv2, 0) != 0)
        throw logic_error("ReseauGTFS::afficherItineraire(): gettimeofday() a échoué pour tv2");
    p_tempsExecution = tempsExecution(tv1, tv2);

    if (tempsDuTrajet == numeric_limits<unsigned int>::max()) {
        if (p_afficherItineraire)
            cout << "La destination n'est pas atteignable de l'orignine durant cet intervalle de temps" << endl;
        return;
    }

    if (tempsDuTrajet == 0) {
        if (p_afficherItineraire) cout << "Vous êtes déjà situé à la destination demandée" << endl;
        return;
    }

    //un chemin non trivial a été trouvé
    if (chemin.size() <= 2)
        throw logic_error("ReseauGTFS::afficherItineraire(): un chemin non trivial doit contenir au moins 3 sommets");
    if (m_arretDuSommet[chemin[0]]->getStationId() != stationIdOrigine)
        throw logic_error("ReseauGTFS::afficherItineraire(): le premier noeud du chemin doit être le point origine");
    if (m_arretDuSommet[chemin[chemin.size() - 1]]->getStationId() != stationIdDestination)
        throw logic_error(
                "ReseauGTFS::afficherItineraire(): le dernier noeud du chemin doit être le point destination");

    if (p_afficherItineraire) {
        std::cout << std::endl;
        std::cout << "=====================" << std::endl;
        std::cout << "     ITINÉRAIRE      " << std::endl;
        std::cout << "=====================" << std::endl;
        std::cout << std::endl;
    }

    if (p_afficherItineraire) cout << "Heure de départ du point d'origine: " << p_gtfs.getTempsDebut() << endl;
    Arret::Ptr ptr_a = m_arretDuSommet.at(chemin[0]);
    Arret::Ptr ptr_b = m_arretDuSommet.at(chemin[1]);
    if (p_afficherItineraire)
        cout << "Rendez vous à la station " << p_gtfs.getStations().at(ptr_b->getStationId()) << endl;

    unsigned int sommet = 1;

    while (sommet < chemin.size() - 1) {
        ptr_a = ptr_b;
        ++sommet;
        ptr_b = m_arretDuSommet.at(chemin[sommet]);
        while (ptr_b->getStationId() == ptr_a->getStationId()) {
            ptr_a = ptr_b;
            ++sommet;
            ptr_b = m_arretDuSommet.at(chemin[sommet]);
        }
        //on a changé de station
        if (ptr_b->getStationId() == stationIdDestination) //cas où on est arrivé à la destination
        {
            if (sommet != chemin.size() - 1)
                throw logic_error(
                        "ReseauGTFS::afficherItineraire(): incohérence de fin de chemin lors d'un changement de station");
            break;
        }
        if (sommet == chemin.size() - 1)
            throw logic_error("ReseauGTFS::afficherItineraire(): on ne devrait pas être arrivé à destination");
        //on a changé de station mais sommet n'est pas le noeud destination
        string voyage_id_a = ptr_a->getVoyageId();
        string voyage_id_b = ptr_b->getVoyageId();
        if (voyage_id_a != voyage_id_b) //on a changé de station à pieds
        {
            if (p_afficherItineraire)
                cout << "De cette station, rendez-vous à pieds à la station "
                     << p_gtfs.getStations().at(ptr_b->getStationId()) << endl;
        } else //on a changé de station avec un voyage
        {
            Heure heure = ptr_a->getHeureArrivee();
            unsigned int ligne_id = p_gtfs.getVoyages().at(voyage_id_a).getLigne();
            string ligne_numero = p_gtfs.getLignes().at(ligne_id).getNumero();
            if (p_afficherItineraire)
                cout << "De cette station, prenez l'autobus numéro " << ligne_numero << " à l'heure " << heure << " "
                     << p_gtfs.getVoyages().at(voyage_id_a) << endl;
            //maintenant allons à la dernière station de ce voyage
            ptr_a = ptr_b;
            ++sommet;
            ptr_b = m_arretDuSommet.at(chemin[sommet]);
            while (ptr_b->getVoyageId() == ptr_a->getVoyageId()) {
                ptr_a = ptr_b;
                ++sommet;
                ptr_b = m_arretDuSommet.at(chemin[sommet]);
            }
            //on a changé de voyage
            if (p_afficherItineraire)
                cout << "et arrêtez-vous à la station " << p_gtfs.getStations().at(ptr_a->getStationId())
                     << " à l'heure "
                     << ptr_a->getHeureArrivee() << endl;
            if (ptr_b->getStationId() == stationIdDestination) //cas où on est arrivé à la destination
            {
                if (sommet != chemin.size() - 1)
                    throw logic_error(
                            "ReseauGTFS::afficherItineraire(): incohérence de fin de chemin lors d'u changement de voyage");
                break;
            }
            if (ptr_a->getStationId() != ptr_b->getStationId()) //alors on s'est rendu à pieds à l'autre station
                if (p_afficherItineraire)
                    cout << "De cette station, rendez-vous à pieds à la station "
                         << p_gtfs.getStations().at(ptr_b->getStationId()) << endl;
        }
    }

    if (p_afficherItineraire) {
        cout << "Déplacez-vous à pieds de cette station au point destination" << endl;
        cout << "Heure d'arrivée à la destination: " << p_gtfs.getTempsDebut().add_secondes(tempsDuTrajet) << endl;
    }
    unsigned int h = tempsDuTrajet / 3600;
    unsigned int reste_sec = tempsDuTrajet % 3600;
    unsigned int m = reste_sec / 60;
    unsigned int s = reste_sec % 60;
    if (p_afficherItineraire) {
        cout << "Durée du trajet: " << h << " heures, " << m << " minutes, " << s << " secondes" << endl;
    }

}


