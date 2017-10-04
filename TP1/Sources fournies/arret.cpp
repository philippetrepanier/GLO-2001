//
// Created by prtos on 29/04/16.
// Modifié par Mario, déc. 2016
//

#include "arret.h"

/*!
 *  \brief Constructeur de la classe Arret
 *  \param[in] p_station_id : identificateur de station
 *  \param[in] p_heure_depart: heure de départ
 *  \param[in] p_heure_arrivee: heure d'arrivée
 *  \param[in] p_numero_sequence: numéro de séquence de l'arrêt dans le voyage
 *  \param[in] p_voyage_id: identificateur du voyage
 *   	Pour votre information le fichier stop_times.txt comprend des données relatives aux arrêts effectués par les autobus ;
 *		il est composé des champs :
 *		- trip_id : identifiant du voyage ;
 *		- arrival_time : spécifie l’heure d’arrivée de l’autobus pour cet arrêt (d’un voyage en particulier) ;
 *		- departure_time : spécifie l’heure de départ de l’autobus pour cet arrêt (d’un voyage en particulier) ;
 *		- stop_id : identifiant unique d’une station ;
 *		- stop_sequence : identifie le numéro de séquence de cet arrêt dans l’ensemble des arrêts effectués pour un voyage en
 *		particulier (il permet ultimement de reconstituer l’ensemble des arrêts effectués pour un voyage dans le bon ordre) ;
 *		- pickup_type : indique si les passagers peuvent embarquer à l’arrêt en question ou si seules les descentes sont
 *		permises ;
 *		- drop_off_type : indique si les passagers sont déposés à l’arrêt selon l’horaire prévu ou que le débarquement n’est
 *		pas disponible.
 *		.
 *		Mais nous n'aurions besoin que de arrival_time (m_heure_arrivee), departure_time(m_heure_depart), stop_id (m_station_id),
 * 		et stop_sequence(m_numero_sequence)
 */
Arret::Arret(unsigned int p_station_id, const Heure &p_heure_arrivee, const Heure &p_heure_depart,
             unsigned int p_numero_sequence, const std::string &p_voyage_id)
        : m_station_id(p_station_id), m_heure_arrivee(p_heure_arrivee), m_heure_depart(p_heure_depart),
          m_numero_sequence(p_numero_sequence), m_voyage_id(p_voyage_id)
{
}


/*!
 * \brief Accesseur de l'attribut m_heure_arrivee
 * \return La valeur courante de l'attribut m_heure_arrivee
 */
const Heure &Arret::getHeureArrivee() const
{
    return m_heure_arrivee;
}

/*!
 * \brief Accesseur de l'attribut m_heure_depart
 * \return La valeur courante de l'attribut m_heure_depart
 */
const Heure &Arret::getHeureDepart() const
{
    return m_heure_depart;
}

/*!
 * \brief Accesseur de l'attribut m_numero_sequence
 * \return La valeur courante de l'attribut m_numero_sequence
 */
unsigned int Arret::getNumeroSequence() const
{
    return m_numero_sequence;
}


/*!
 * \brief Accesseur de l'attribut m_station_id
 * \return La valeur courante de l'attribut m_station_id
 */
unsigned int Arret::getStationId() const
{
    return m_station_id;
}


/*!
 * \brief Défini si l'arret courant précède celui passé en paramètre
 * \param[in] p_other: le membre de droit de la comparaison
 */
bool Arret::operator<(const Arret &p_other) const
{
    return m_heure_depart < p_other.getHeureArrivee();
}

/*!
 * \brief Défini si l'arret courant succède celui passé en paramètre
 * \param[in] p_other: le membre de droit de la comparaison
 */
bool Arret::operator>(const Arret &p_other) const
{
    return m_heure_arrivee > p_other.getHeureDepart();
}


/*!
 * \brief Opérateur de flux permettant d'afficher l'heure d'arrivée du bus pour cet arret
 * param[out] flux: le flux de sortie pour afficher
 * param[in] p_arret: l'arret à afficher
 * \return le flux de sortie mis à jour
 */
std::ostream &operator<<(std::ostream &flux, const Arret &p_arret)
{
    flux << p_arret.m_heure_arrivee;
    return flux;
}

std::string Arret::getVoyageId() const
{
    return m_voyage_id;
}

