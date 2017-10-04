//
// Created by prtos on 29/04/16.
// Modifié par Mario, déc, 2016
//

#include "voyage.h"

/*!
 * \brief Constructeur de la classes Voyage
 * \param[in] p_id : identificateur du voyage
 * \param[in] p_ligne_id : identificateur de la ligne desservie par le voyage
 * \param[in] p_service_id: identificateur du service auquel ce voyage appartient
 * \param[in] p_destination: destination du voyage
 */
Voyage::Voyage(const std::string &p_id, unsigned int p_ligne_id, const std::string &p_service_id,
               const std::string &p_destination) :
        m_id(p_id), m_ligne(p_ligne_id), m_service_id(p_service_id), m_destination(p_destination)
{
}

Voyage::Voyage()
{
}

//! \brief retourne le conteneur m_arrets par référence constante
const std::set<Arret::Ptr, Voyage::compArret> &Voyage::getArrets() const
{
    return m_arrets;
}

const std::string &Voyage::getDestination() const
{
    return m_destination;
}

std::string Voyage::getId() const
{
    return m_id;
}

unsigned int Voyage::getLigne() const
{
    return m_ligne;
}

std::string Voyage::getServiceId() const
{
    return m_service_id;
}

/*!
 * \brief retourne l'heure de départ du voyage, ie l'heure d'arrivée du premier arret dans m_arret
 * \return l'heure de départ
 * \exception std::logic_error s'il n'y a aucun arret pour ce voyage
 * \post m_arret est initialisé grâce à setArret
 */
Heure Voyage::getHeureDepart() const
{
    if (m_arrets.size() == 0) throw std::logic_error("aucun arret pour ce voyage");
    return (*(m_arrets.begin()))->getHeureArrivee();
}

/*!
 * \brief retourne l'heure de fin du voyage, ie l'heure de d'arrivée du dernier arret dans m_arret
 * \return l'heure de fin
 * \exception std::logic_error s'il n'y a aucun arret pour ce voyage
 * \post m_arret est initialisé grâce à setArret
 */
Heure Voyage::getHeureFin() const
{
    if (m_arrets.size() == 0) throw std::logic_error("aucun arret pour ce voyage");
    return (*(--m_arrets.end()))->getHeureArrivee();
}

void Voyage::ajouterArret(const Arret::Ptr &p_arret)
{
    m_arrets.insert(p_arret);
}


/*!
 * \brief Inégalité inférieure entre deux voyages.
 * Cet opérateur est utile pour savoir si le voyage courant commence avant celui avec qui on le compare.
 * \param[in] p_other: l'autre voyage avec qui l'on se compare
 * \return True ssi les l'Heure de départ de l'objet courant est inférieur  à celle de l'objet en paramètre.
 */
bool Voyage::operator<(const Voyage &p_other) const
{
    return getHeureDepart() < p_other.getHeureDepart();
}

/*!
 * \brief Inégalité supérieure entre deux voyages.
 * Cet opérateur est utile pour savoir si le voyage courant commence avant celui avec qui on le compare.
 * \param[in] p_other: l'autre voyage avec qui l'on se compare
 * \return True ssi les l'Heure de départ de l'objet courant est inférieur  à celle de l'objet en paramètre.
 */
bool Voyage::operator>(const Voyage &p_other) const
{
    return getHeureDepart() > p_other.getHeureDepart();
}

/*!
 * \brief Permet l'affichage d'un voyage au format "(%Numero_ligne_effetuant_voyant : Vers %destination)"
 * \param[in,out] flux: le flux de sortie utilisé pour l'affichage
 * \param[in] p_voyage: le voyage à afficher
 * \return le flux de sortie mis à jour
 */
std::ostream &operator<<(std::ostream &flux, const Voyage &p_voyage)
{
    flux << "Vers " << p_voyage.getDestination();
    return flux;
}

unsigned int Voyage::getNbArrets() const
{
    return (unsigned int) m_arrets.size();
}

//! \brief //foncteur de comparaison pour les arrets de m_arrets
bool Voyage::compArret::operator()(Arret::Ptr i, Arret::Ptr j) const
{
    bool rep = i->getNumeroSequence() < j->getNumeroSequence();
    if (rep && i->getHeureDepart() > j->getHeureArrivee())
        throw std::logic_error("Incohérence des numéros de séquences avec les heures");
    return rep;
}
