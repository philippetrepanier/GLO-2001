//
// Created by prtos on 29/04/16.
//

#include "coordonnees.h"

/*!
 * \brief Constructeur de la classe, permet de construire une coordonnéees à partir de la longitude et de la latitude.
 * \exception logic_error si La latitude et/ou la longitude est invalide
 */
Coordonnees::Coordonnees(double latitude, double longitude) : m_latitude(latitude), m_longitude(longitude)
{
    if (!Coordonnees::is_valide_coord(latitude, longitude))
    {
        throw std::logic_error("La latitude ou la longitude est invalide");
    }

};

/*!
 * \brief Constructeur de copie de la classe, permet de construire une coordonnéees à partir d'une autre coordonnée.
 * \exception logic_error si La latitude et/ou la longitude de cette coordonnée est invalide
 */
Coordonnees::Coordonnees(const Coordonnees & rhs)
{
    if (this != &rhs)
    {
        double longitude = rhs.getLongitude();
        double latitude = rhs.getLatitude();
        if (!Coordonnees::is_valide_coord(latitude, longitude))
        {
            throw std::logic_error("Coordonnees::Coordonnees(): rhs est une coordonnée invalide");
        }
        m_longitude = rhs.getLongitude();
        m_latitude = rhs.getLatitude();
    }
}

double Coordonnees::getLatitude() const
{
    return m_latitude;
};

double Coordonnees::getLongitude() const
{
    return m_longitude;
};

/*!
 * \brief : Cette fonction vérifie si les longitude et latitude en argument représentent
 * une coordonnée gps valide. Voir https://en.wikipedia.org/wiki/Geographic_coordinate_system
 * \param[in] p_latitude : La latitude du point à tester
 * \param[in] p_longitude : la longitude du point à tester
 * \return True ssi les deux arguments permettent de former une coordonnées GPS valide.
 */
bool Coordonnees::is_valide_coord(double p_latitude, double p_longitude)
{
    return p_latitude >= 0.0 && p_latitude <= 90.0
           && p_longitude >= -180.0 && p_longitude <= 180.0;
};

/*!
 * \brief Cet opérateur calcule la distance entre la coordonnée courante et celle passée en paramètre.
 * La formule de calcul est disponible à l'adresse https://en.wikipedia.org/wiki/Great-circle_distance. \n
 * Notez qu'il s'agit d'une approximation de la distance à vol d'oiseau et donc pas très réaliste dans certains cas.
 * \param[in] other: est la seconde coordonnée gps
 * \return La distance, en km, entre les deux coordonnées
 */
double Coordonnees::operator-(const Coordonnees &other) const
{
    double radParDegre = 3.14159265358979323846 / 180.0;
    double rayonTerre = 6371; //en km
    double lon1 = m_longitude * radParDegre;
    double lat1 = m_latitude * radParDegre;
    double lon2 = other.m_longitude * radParDegre;
    double lat2 = other.m_latitude * radParDegre;
    double res = rayonTerre * acos(cos(lat1) * cos(lat2) * cos(lon2 - lon1) + sin(lat1) * sin(lat2));
    return res;
};

/*!
 * \brief Permet l'affichage d'une coordonnée au format "(lat: %latitude, long: %longitude)"
 * \param[in,out] flux: le flux de sortie utilisé pour l'affichage
 * \param[in] p_coord: la coordonnée à afficher
 * \return le flux de sortie mis à jour
 */
std::ostream &operator<<(std::ostream &flux, const Coordonnees &p_coord)
{
    flux << "(lat:";
    flux << p_coord.m_latitude;
    flux << ", long:";
    flux << p_coord.m_longitude;
    flux << ")";
    return flux;
}
