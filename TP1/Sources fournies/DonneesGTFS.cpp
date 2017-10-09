//
// Created by Mario Marchand on 16-12-29.
// Modified by Philippe Trépanier on 17-10-09
//

#include "DonneesGTFS.h"

using namespace std;

//! \brief construit un objet GTFS
//! \param[in] p_date: la date utilisée par le GTFS
//! \param[in] p_now1: l'heure du début de l'intervalle considéré
//! \param[in] p_now2: l'heure de fin de l'intervalle considéré
//! \brief Ces deux heures définissent l'intervalle de temps du GTFS; seuls les moments de [p_now1, p_now2) sont considérés
DonneesGTFS::DonneesGTFS(const Date &p_date, const Heure &p_now1, const Heure &p_now2)
        : m_date(p_date), m_now1(p_now1), m_now2(p_now2), m_nbArrets(0), m_tousLesArretsPresents(false) {
}

//! \brief partitionne un string en un vecteur de strings
//! \param[in] s: le string à être partitionner
//! \param[in] delim: le caractère utilisé pour le partitionnement
//! \return le vecteur de string sans le caractère utilisé pour le partitionnement
vector<string> DonneesGTFS::string_to_vector(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> elems;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

//! \brief ajoute les lignes dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les lignes
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterLignes(const std::string &p_nomFichier) {
    // Ouverture du fichier
    ifstream fichier(p_nomFichier, ios::in);
    string ligneFich;

    if (!fichier.is_open()) {
        throw logic_error("Erreur d'ouverture du fichier");
    }

    getline(fichier, ligneFich); // ENTETE

    while (getline(fichier, ligneFich)) {
        // Enlever les " " de la chaine du string
        ligneFich.erase(remove(ligneFich.begin(), ligneFich.end(), '\"'), ligneFich.end());

        // Découper le string dans un vecteur
        vector<string> route = string_to_vector(ligneFich, ',');

        // Insérer les éléments dans les containers
        m_lignes.insert(
                {stoul(route[0]), Ligne(stoul(route[0]), route[2], route[4], Ligne::couleurToCategorie(route[7]))});
        m_lignes_par_numero.insert(
                {route[2], Ligne(stoul(route[0]), route[2], route[4], Ligne::couleurToCategorie(route[7]))});
    }

    fichier.close();
}

//! \brief ajoute les stations dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterStations(const std::string &p_nomFichier) {
    // Ouverture du fichier
    ifstream fichier(p_nomFichier, ios::in);
    string ligneFich;

    if (!fichier.is_open()) {
        throw logic_error("Erreur d'ouverture du fichier");
    }

    getline(fichier, ligneFich); // ENTETE

    while (getline(fichier, ligneFich)) {
        // Enlever les " " de la chaine du string
        ligneFich.erase(remove(ligneFich.begin(), ligneFich.end(), '\"'), ligneFich.end());

        // Découper le string dans un vecteur
        vector<string> stationVect = string_to_vector(ligneFich, ',');

        // Insérer les éléments dans les containers
        m_stations.insert({stoul(stationVect[0]), Station(stoul(stationVect[0]), stationVect[1], stationVect[2],
                                                          Coordonnees(stod(stationVect[3]), stod(stationVect[4])))});

    }

    fichier.close();
}

//! \brief ajoute les transferts dans l'objet GTFS
//! \breif Cette méthode doit âtre utilisée uniquement après que tous les arrêts ont été ajoutés
//! \brief les transferts (entre stations) ajoutés sont uniquement ceux pour lesquelles les stations sont prensentes dans l'objet GTFS
//! \brief les transferts d'une station à elle m^e ne sont pas ajoutés
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
//! \throws logic_error si tous les arrets de la date et de l'intervalle n'ont pas été ajoutés
void DonneesGTFS::ajouterTransferts(const std::string &p_nomFichier) {
    if (m_tousLesArretsPresents) {
        // Ouverture du fichier
        ifstream fichier(p_nomFichier, ios::in);
        string ligneFich;

        if (!fichier.is_open()) {
            throw logic_error("Erreur d'ouverture du fichier");
        }
        if (!m_tousLesArretsPresents) {
            throw logic_error("Les arrets de la date/intervalle n'ont pas été ajoutés!");
        }

        getline(fichier, ligneFich); // ENTETE

        while (getline(fichier, ligneFich)) {
            // Découper le string dans un vecteur
            vector<string> transfertVect = string_to_vector(ligneFich, ',');
            if (transfertVect[0] == transfertVect[1]) {
                continue;
            }
            if ((m_stations.find(stoul(transfertVect[0])) != m_stations.end()) and
                (m_stations.find(stoul(transfertVect[1])) != m_stations.end())) {
                if (transfertVect[3] == "0\r") {
                    transfertVect[3] = "1";
                }
                m_transferts.push_back(
                        make_tuple(stoul(transfertVect[0]), stoul(transfertVect[1]), stoul(transfertVect[3])));
            }
        }

        fichier.close();
    }
}


//! \brief ajoute les services de la date du GTFS (m_date)
//! \param[in] p_nomFichier: le nom du fichier contenant les services
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterServices(const std::string &p_nomFichier) {
    // Ouverture du fichier
    ifstream fichier(p_nomFichier, ios::in);
    string ligneFich;

    if (!fichier.is_open()) {
        throw logic_error("Erreur d'ouverture du fichier");
    }

    getline(fichier, ligneFich); // ENTETE

    while (getline(fichier, ligneFich)) {
        // Enlever les " " de la chaine du string
        ligneFich.erase(remove(ligneFich.begin(), ligneFich.end(), '\"'), ligneFich.end());

        // Découper le string dans un vecteur
        vector<string> servicesVect = string_to_vector(ligneFich, ',');

        //Définir la date en cours
        Date dateServ(stoul(servicesVect[1].substr(0, 4)), stoul(servicesVect[1].substr(4, 2)),
                      stoul(servicesVect[1].substr(6, 2)));

        if (dateServ == m_date) {
            if (servicesVect[2] == "1") {
                m_services.insert(servicesVect[0]);
            }
        }
    }

    fichier.close();
}

//! \brief ajoute les voyages de la date
//! \brief seuls les voyages dont le service est présent dans l'objet GTFS sont ajoutés
//! \param[in] p_nomFichier: le nom du fichier contenant les voyages
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterVoyagesDeLaDate(const std::string &p_nomFichier) {
    // Ouverture du fichier
    ifstream fichier(p_nomFichier, ios::in);
    string ligneFich;

    if (!fichier.is_open()) {
        throw logic_error("Erreur d'ouverture du fichier");
    }

    getline(fichier, ligneFich); // ENTETE

    while (getline(fichier, ligneFich)) {
        // Enlever les " " de la chaine du string
        ligneFich.erase(remove(ligneFich.begin(), ligneFich.end(), '\"'), ligneFich.end());

        // Découper le string dans un vecteur
        vector<string> tripsVect = string_to_vector(ligneFich, ',');

        if (m_services.find(tripsVect[1]) != m_services.end()) {
            m_voyages.insert({tripsVect[2], Voyage(tripsVect[2], stoul(tripsVect[0]), tripsVect[1], tripsVect[3])});
        }
    }

    fichier.close();
}

//! \brief ajoute les arrets aux voyages présents dans le GTFS si l'heure du voyage appartient à l'intervalle de temps du GTFS
//! \brief De plus, on enlève les voyages qui n'ont pas d'arrêts dans l'intervalle de temps du GTFS
//! \brief De plus, on enlève les stations qui n'ont pas d'arrets dans l'intervalle de temps du GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les arrets
//! \post assigne m_tousLesArretsPresents à true
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterArretsDesVoyagesDeLaDate(const std::string &p_nomFichier) {
    // Ouverture du fichier
    ifstream fichier(p_nomFichier, ios::in);
    string ligneFich;

    if (!fichier.is_open()) {
        throw logic_error("Erreur d'ouverture du fichier");
    }

    getline(fichier, ligneFich); // ENTETE

    while (getline(fichier, ligneFich)) {
        // Enlever les " " de la chaine du string
        ligneFich.erase(remove(ligneFich.begin(), ligneFich.end(), '\"'), ligneFich.end());

        // Découper le string dans un vecteur
        vector<string> stopVect = string_to_vector(ligneFich, ',');

        if (m_voyages.find(stopVect[0]) != m_voyages.end()) {
            Heure heureArrive(stoul(stopVect[1].substr(0, 2)), stoul(stopVect[1].substr(3, 2)),
                              stoul(stopVect[1].substr(6, 2)));
            Heure heureDepart(stoul(stopVect[2].substr(0, 2)), stoul(stopVect[2].substr(3, 2)),
                              stoul(stopVect[2].substr(6, 2)));

            if (m_now1 <= heureDepart and heureArrive < m_now2) {
                Arret::Ptr a_ptr = make_shared<Arret>(stoul(stopVect[3]), heureArrive, heureDepart, stoul(stopVect[4]),
                                                      stopVect[0]);

                m_voyages[stopVect[0]].ajouterArret(a_ptr);
                m_stations[stoul(stopVect[3])].addArret(a_ptr);
                ++m_nbArrets;
            }
        }
    }

    // Nettoyer les voyages vides
    map<string, Voyage>::iterator it;

    it = m_voyages.begin();

    while (it != m_voyages.end()) {
        if (it->second.getNbArrets() < 1) {
            it = m_voyages.erase(it);
        } else {
            ++it;
        }
    }

    map<unsigned int, Station>::iterator it2;

    for (it2 = m_stations.begin(); it2 != m_stations.end(); it2) {
        if (it2->second.getNbArrets() < 1) {
            it2 = m_stations.erase(it2);
        } else {
            ++it2;
        }
    }

    fichier.close();
    m_tousLesArretsPresents = true;
}

unsigned int DonneesGTFS::getNbArrets() const {
    return m_nbArrets;
}

size_t DonneesGTFS::getNbLignes() const {
    return m_lignes.size();
}

size_t DonneesGTFS::getNbStations() const {
    return m_stations.size();
}

size_t DonneesGTFS::getNbTransferts() const {
    return m_transferts.size();
}

size_t DonneesGTFS::getNbServices() const {
    return m_services.size();
}

size_t DonneesGTFS::getNbVoyages() const {
    return m_voyages.size();
}

void DonneesGTFS::afficherLignes() const {
    std::cout << "======================" << std::endl;
    std::cout << "   LIGNES GTFS   " << std::endl;
    std::cout << "   COMPTE = " << m_lignes.size() << "   " << std::endl;
    std::cout << "======================" << std::endl;
    for (const auto &ligneM : m_lignes_par_numero) {
        cout << ligneM.second;
    }
    std::cout << std::endl;
}

void DonneesGTFS::afficherStations() const {
    std::cout << "========================" << std::endl;
    std::cout << "   STATIONS GTFS   " << std::endl;
    std::cout << "   COMPTE = " << m_stations.size() << "   " << std::endl;
    std::cout << "========================" << std::endl;
    for (const auto &stationM : m_stations) {
        std::cout << stationM.second << endl;
    }
    std::cout << std::endl;
}

void DonneesGTFS::afficherTransferts() const {
    std::cout << "========================" << std::endl;
    std::cout << "   TRANSFERTS GTFS   " << std::endl;
    std::cout << "   COMPTE = " << m_transferts.size() << "   " << std::endl;
    std::cout << "========================" << std::endl;
    for (unsigned int i = 0; i < m_transferts.size(); ++i) {
        std::cout << "De la station " << get<0>(m_transferts.at(i)) << " vers la station " << get<1>(m_transferts.at(i))
                  <<
                  " en " << get<2>(m_transferts.at(i)) << " secondes" << endl;

    }
    std::cout << std::endl;

}


void DonneesGTFS::afficherArretsParVoyages() const {
    std::cout << "=====================================" << std::endl;
    std::cout << "   VOYAGES DE LA JOURNÉE DU " << m_date << std::endl;
    std::cout << "   " << m_now1 << " - " << m_now2 << std::endl;
    std::cout << "   COMPTE = " << m_voyages.size() << "   " << std::endl;
    std::cout << "=====================================" << std::endl;

    for (const auto &voyageM : m_voyages) {
        unsigned int ligne_id = voyageM.second.getLigne();
        auto l_itr = m_lignes.find(ligne_id);
        cout << (l_itr->second).getNumero() << " ";
        cout << voyageM.second << endl;
        for (const auto &a: voyageM.second.getArrets()) {
            unsigned int station_id = a->getStationId();
            auto s_itr = m_stations.find(station_id);
            std::cout << a->getHeureArrivee() << " station " << s_itr->second << endl;
        }
    }

    std::cout << std::endl;
}

void DonneesGTFS::afficherArretsParStations() const {
    std::cout << "========================" << std::endl;
    std::cout << "   ARRETS PAR STATIONS   " << std::endl;
    std::cout << "   Nombre d'arrêts = " << m_nbArrets << std::endl;
    std::cout << "========================" << std::endl;
    for (const auto &stationM : m_stations) {
        std::cout << "Station " << stationM.second << endl;
        for (const auto &arretM : stationM.second.getArrets()) {
            string voyage_id = arretM.second->getVoyageId();
            auto v_itr = m_voyages.find(voyage_id);
            unsigned int ligne_id = (v_itr->second).getLigne();
            auto l_itr = m_lignes.find(ligne_id);
            std::cout << arretM.first << " - " << (l_itr->second).getNumero() << " " << v_itr->second << std::endl;
        }
    }
    std::cout << std::endl;
}

const std::map<std::string, Voyage> &DonneesGTFS::getVoyages() const {
    return m_voyages;
}

const std::map<unsigned int, Station> &DonneesGTFS::getStations() const {
    return m_stations;
}

const std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > &DonneesGTFS::getTransferts() const {
    return m_transferts;
}

Heure DonneesGTFS::getTempsFin() const {
    return m_now2;
}

Heure DonneesGTFS::getTempsDebut() const {
    return m_now1;
}

const std::unordered_map<unsigned int, Ligne> &DonneesGTFS::getLignes() const {
    return m_lignes;
}



