//
// Created by prtos on 29/04/16.
// Modifé par Mario, déc. 2016
//

#include "auxiliaires.h"

using namespace std;;

/*!
 * \brief Constructeur par défaut de la classe.
 * Permet d'initialiser un objet Date qui est la date actuelle
 *
 */
Date::Date()
{
    time_t lt = time(nullptr);   //epoch seconds
    struct tm *p = localtime(&lt);
    m_an = (unsigned int) (p->tm_year + 1900);
    m_mois = (unsigned int) (p->tm_mon + 1);
    m_jour = (unsigned int) (p->tm_mday);
    encode(m_an, m_mois, m_jour);
}

/*!
 * \brief Constructeur avec paramètres de la classe.
 * Permet d'initialiser une date avec les paramètres en argument
 * \param[in] p_an: l'année dela date
 * \param[in] p_mois: le mois de la date
 * \param[in] p_jour: le jour de la date
 *
 */
Date::Date(unsigned int p_an, unsigned int p_mois, unsigned int p_jour)
{
    m_an = p_an;
    m_mois = p_mois;
    m_jour = p_jour;
    encode(p_an, p_mois, p_jour);
}

/*!
 * \brief Égalité entre deux dates
 * Deux dates sont égales s'ils ont la même année, le même mois, le même jour
 * \param[in] other: l'autre date avec qui l'on se compare
 * \return True si les deux dates sont égales.
 *
 */
bool Date::operator==(const Date &other) const
{
    return m_code == other.m_code;
}

/*!
 * \brief Inégalité inférieure entre deux dates
 * \param[in] other: l'autre date avec qui l'on se compare
 * \return True si l'objet courant est inférieure à l'objet passé en paramètre, False autrement
 */
bool Date::operator<(const Date &other) const
{
    return m_code < other.m_code;
}

/*!
 * \brief Inégalité supérieure entre deux dates
 * \param[in] other: l'autre date avec qui l'on se compare
 * \return False si l'objet courant est inférieure à l'objet passé en paramètre, True autrement
 */
bool Date::operator>(const Date &other) const
{
    return m_code > other.m_code;
}

/*!
 * \brief Permet de déterminer le code d'une date, i.e le nombre de jours depuis 1970-01-01
 * \param[in] an: l'année dela date
 * \param[in] mois: le mois de la date
 * \param[in] jour: le jour de la date
 *
 */
void Date::encode(unsigned int an, unsigned int mois, unsigned int jour)
{
    mois -= 2;
    if (mois <= 0)
    {
        mois += 12;
        an -= 1;
    }
    m_code = an / 4 - an / 100 + an / 400 + 367 * mois / 12 + jour;
    m_code = m_code + 365 * an - 719499;
}

/*!
 * \brief Permet l'affichage d'une date au format AAAA-MM-JJ
 * \param[in,out] flux: le flux de sortie utilisé pour l'affichage
 * \param[in] p_date: la date à afficher
 * \return le flux de sortie mis à jour
 */
std::ostream &operator<<(std::ostream &flux, const Date &p_date)
{
    flux << p_date.m_an << "-";

    if (p_date.m_mois < 10)
    {
        flux << "0" << p_date.m_mois << "-";
    } else
    {
        flux << p_date.m_mois << "-";
    }

    if (p_date.m_jour < 10)
    {
        flux << "0" << p_date.m_jour;
    } else
    {
        flux << p_date.m_jour;
    }

    return flux;
}

/*!
 * \brief Constructeur par défaut de la classe Heure. Instancie une Heure avec l'heure actuelle
 *
 */
Heure::Heure()
{
    time_t lt = time(nullptr);   //epoch seconds
    struct tm *p = localtime(&lt);
    m_heure = (unsigned int) (p->tm_hour);
    m_min = (unsigned int) (p->tm_min);
    m_sec = (unsigned int) (p->tm_sec);
    encode(m_heure, m_min, m_sec);
}

/*!
 * \brief Constructeur avec paramètre de la classe Heure. Instancie une Heure avec les paramètres.
 * \param[in] p_heure: le nombre d'heure de l'heure à instancier
 * \param[in] p_min: le nombre de minutes dans l'heure
 * \param[in] p_sec: le nombre de secondes dans la minute
 */
Heure::Heure(unsigned int p_heure, unsigned int p_min, unsigned int p_sec)
{
    m_heure = p_heure;
    m_min = p_min;
    m_sec = p_sec;
    encode(m_heure, m_min, m_sec);
}

/*!
 * \brief Égalité entre deux heures
 * Deux heures sont égales s'ils ont la même heure, la même minute et la même seconde
 * \param[in] other: l'autre heure avec qui l'on se compare
 * \return True ssi les deux Heures sont égales.
 *
 */
bool Heure::operator==(const Heure &other) const
{
    return m_code == other.m_code;
}

/*!
 * \brief Inégalité inférieure stricte entre deux heures
 * \param[in] other: l'autre heure avec qui l'on se compare
 * \return True ssi les l'Heure de l'objet courant est inférieur à celle de l'objet en paramètre.
 */
bool Heure::operator<(const Heure &other) const
{
    return m_code < other.m_code;
}

/*!
 * \brief Inégalité supérieure stricte entre deux heures
 * \param[in] other: l'autre heure avec qui l'on se compare
 * \return True ssi les l'Heure de l'objet courant est supérieur à celle de l'objet en paramètre.
 */
bool Heure::operator>(const Heure &other) const
{
    return m_code > other.m_code;
}

/*!
 * \brief Inégalité inférieure entre deux heures
 * \param[in] other: l'autre heure avec qui l'on se compare
 * \return True ssi les l'Heure de l'objet courant est inférieur ou égale à celle de l'objet en paramètre.
 */
bool Heure::operator<=(const Heure &other) const
{
    return m_code <= other.m_code;
}

/*!
 * \brief Inégalité supérieure entre deux heures
 * \param[in] other: l'autre heure avec qui l'on se compare
 * \return True ssi les l'Heure de l'objet courant est supérieur ou égale à celle de l'objet en paramètre.
 */
bool Heure::operator>=(const Heure &other) const
{
    return m_code >= other.m_code;
}

/*!
 * \brief Différence entre deux heures
 * \param[in] other: l'autre heure avec qui l'on fait l'opération
 * \return le nombre de secondes (positif ou négatif) qui sépare les deux heures.
 */
int Heure::operator-(const Heure &other) const
{
    return m_code - other.m_code;
}

/*!
 * \brief Ajoute un certain nombre de secondes à l'heure de l'objet courant
 * \param[in] secs: le nombre de secondes à ajouter
 * \return la nouvelle heure obtenue après l'ajout des secondes
 */
Heure Heure::add_secondes(unsigned int secs) const
{
    unsigned int code = m_code + secs; //la nouvelle heure en secondes
    unsigned int h = code / 3600; //le nombre d'heures de cette nouvelle heure
    unsigned int reste = code % 3600; //le reste en secondes
    unsigned int m = reste / 60; //le nombre de minutes de cette nouvelle heure
    unsigned int s = reste % 60; //le nombre de secondes de cette nouvelle heure
    return Heure(h, m, s);
}

/*!
 * \brief Encode et modifie l'attribut hour_code de l'objet. L'encodage revient à déterminier le nombre de secondes depuis 00h00m00s
 * \param[in] p_heure: le nombre d'heure de l'heure à instancier
 * \param[in] p_min: le nombre de minutes dans l'heure
 * \param[in] p_sec: le nombre de secondes dans la minute
 */
void Heure::encode(unsigned int p_heure, unsigned int p_min, unsigned int p_sec)
{
    m_code = (((60 * p_heure) + p_min) * 60) + p_sec;
}

/*!
 * \brief Permet l'affichage d'une heure au format HH:MM:SS
 * \param[in,out] flux: le flux de sortie utilisé pour l'affichage
 * \param[in] p_heure: l'heure à afficher
 * \return le flux de sortie mis à jour
 */
std::ostream &operator<<(std::ostream &flux, const Heure &p_heure)
{
    if (p_heure.m_heure < 10)
    {
        flux << "0" << p_heure.m_heure << ":";
    } else
    {
        flux << p_heure.m_heure << ":";
    }

    if (p_heure.m_min < 10)
    {
        flux << "0" << p_heure.m_min << ":";
    } else
    {
        flux << p_heure.m_min << ":";
    }

    if (p_heure.m_sec < 10)
    {
        flux << "0" << p_heure.m_sec;
    } else
    {
        flux << p_heure.m_sec;
    }
    return flux;
}
