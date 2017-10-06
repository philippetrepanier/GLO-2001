//
// Created by prtos on 29/04/16.
// Modifié par Mario, déc 2016
//

#include "station.h"

/*!
 * \brief Constructeur de la classe Station.
 * \param[in] ligne_gtfs : vecteur de strings représentant une ligne du fichier stops.txt. \n
 * Pour votre information, il est composé des champs suivants :
 * 	- stop_id : identifiant unique d’une station ;
 * 	- stop_name : contient le nom de la station ;
 * 	- stop_desc : contient la description de la station ;
 * 	- stop_lat : contient la latitude de la station au format wgs84 (voir https://fr.wikipedia.org/wiki/WGS_84.);
 * 	- stop_lon : contient la longitude de la station au format wgs84 ;
 * 	- stop_url : contient l’URL d’une page décrivant une station en particulier ;
 * 	- location_type : différencie les stations entre-elles ; si ce champ est vide, ce qui est le cas dans les données publiées par le RTC, c’est une station par défaut ;
 * 	- wheelchair_boarding : identifie si l’embarquement de fauteuil roulant est possible à l’arrêt ; une valeur
 * 		- 0 (ou vide) indique que cette information n’est pas disponible pour cette station ;
 * 		- 1 indique qu’un véhicule passant par cette station peut accueillir au moins un fauteuil roulant ;
 * 		- 2 indique qu’aucun fauteuil roulant ne peut être pris en charge à cette station.
 * 		.
 * 	.
 * Dans le cadre de ce travail, nous n’utiliserons qu’une partie de ces données ; plus précisément stop_id (m_id), stop_name(m_nom), stop_desc(m_description), stop_lat et stop_long(m_coords)
 *
 */
Station::Station(unsigned int p_id, const std::string &p_nom, const std::string &p_description,
                 const Coordonnees &p_coords) :
        m_id(p_id), m_nom(p_nom), m_description(p_description), m_coords(p_coords)
{

}

Station::Station() : m_coords(0, 0)
{

}

/*!
 * \brief Permet l'affichage d'une station au format "%identifiant - %description"
 * \param[in,out] flux: le flux de sortie utilisé pour l'affichage
 * \param[in] p_station: la station à afficher
 * \return le flux de sortie mis à jour
 */
std::ostream &operator<<(std::ostream &flux, const Station &p_station)
{
    flux << p_station.m_id;
    flux << " - " << p_station.m_nom << " " << p_station.m_coords;
    return flux;
}

const Coordonnees &Station::getCoords() const
{
    return m_coords;
}

const std::string &Station::getDescription() const
{
    return m_description;
}

const std::string &Station::getNom() const
{
    return m_nom;
}

unsigned int Station::getId() const
{
    return m_id;
}

void Station::addArret(const Arret::Ptr &p_arret)
{
    m_arrets.insert({p_arret->getHeureArrivee(), p_arret});
}

//! \brief retourne le conteneur m_arrets par référence constante
const std::multimap<Heure, Arret::Ptr> &Station::getArrets() const
{
    return m_arrets;
}

unsigned int Station::getNbArrets() const
{
    return (unsigned int) m_arrets.size();
}







