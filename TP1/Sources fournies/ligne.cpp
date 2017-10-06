//
// Created by prtos on 29/04/16.
// Modifié par Mario on 21/11/16

#include "ligne.h"

/*!
 * \brief Constructeur de la classe ligne
 * \param[in] p_id : route_id : identifiant unique de la ligne d’autobus
 * \param[in] p_numero : il s’agit du numéro de la ligne ("7", "800", "801", "13A", "13B", etc.)
 * \param[in] p_description: texte décrivant la ligne ; le RTC a choisi de préciser les noms des terminaux
 * \param[in] p_categorie: CategorieBus (venant de couleur) permettant d’identifier visuellement la ligne
 * Nous n'utilisons que route_id (m_id), route_short_name (m_numero), route_desc(m_description), route_color(m_categorie) du fichier routes.txt
 *  L'attribut m_voyages n'est pas initialisé
 */
Ligne::Ligne(unsigned int p_id, const std::string &p_numero, const std::string &p_description,
             const CategorieBus &p_categorie) :
        m_id(p_id), m_numero(p_numero), m_description(p_description), m_categorie(p_categorie)
{
}

Ligne::Ligne() : m_id(), m_numero(""), m_description(""), m_categorie(CategorieBus::METRO_BUS)
{
}

/*!
 * \brief Méthode statique permettant de convertir de trouver la catégorie de bus à partir de sa couleur. \n
 * La correspondance des couleurs est la suivante:
 * - 97BF0D (Verte)--> Métro bus
 * - 013888 (Bleue) --> Le bus
 * - E04503 (Orange) --> Express
 * - 1A171B (Noir) ou 003888 (Bleue) --> Couche tard
 * \param[in] couleur: La couleur d'intérêt
 * \exception logic_error si la couleur ne correspond à aucun bus
 * \return La catégorie déduite
 */
CategorieBus Ligne::couleurToCategorie(const std::string &couleur)
{
    CategorieBus c;
    if (couleur == "97BF0D")
    {
        c = CategorieBus::METRO_BUS;
    } else if (couleur == "013888")
    {
        c = CategorieBus::LEBUS;
    } else if (couleur == "E04503")
    {
        c = CategorieBus::EXPRESS;
    } else if ((couleur == "1A171B") || (couleur == "003888"))
    {
        c = CategorieBus::COUCHE_TARD;
    } else
    {
        std::cout << couleur << std::endl;
        throw std::logic_error("Couleur " + couleur + " non répertorié");
    }
    return c;
}

/*!
 * \brief Méthode statique permettant de correspondre une chaine de caractère à une catégorie de bus.\n
 * On associe CategorieBus::METRO_BUS à "METRO BUS", CategorieBus::LEBUS à "LE BUS", etc
 * \param[in] c: La catégorie à faire correspondre
 * \exception logic_error si la catégorie est pas référencé dans CategorieBus
 * \return une chaine de caractère associée
 */
std::string Ligne::categorieToString(const CategorieBus &c)
{
    std::string couleur;
    if (c == CategorieBus::METRO_BUS)
    {
        couleur = "METRO_BUS";
    } else if (c == CategorieBus::LEBUS)
    {
        couleur = "LEBUS";
    } else if (c == CategorieBus::EXPRESS)
    {
        couleur = "EXPRESS";
    } else if (c == CategorieBus::COUCHE_TARD)
    {
        couleur = "COUCHE_TARD";
    } else
    {
        throw "Catégorie de bus non répertorié";
    }
    return couleur;
}

/*!
 * \brief Permet l'affichage d'une ligne au format " %Categorie %numero_du_bus : %description"
 * \param[in,out] f: le flux de sortie utilisé pour l'affichage
 * \param[in] p_ligne: la ligne à afficher
 * \return le flux de sortie mis à jour
 */
std::ostream &operator<<(std::ostream &f, const Ligne &p_ligne)
{
    f << Ligne::categorieToString(p_ligne.m_categorie) << " " << p_ligne.m_numero << " : " << p_ligne.m_description
      << std::endl;
    return f;
}

CategorieBus Ligne::getCategorie() const
{
    return m_categorie;
}

unsigned int Ligne::getId() const
{
    return m_id;
}

std::string Ligne::getNumero() const
{
    return m_numero;
}

const std::string &Ligne::getDescription() const
{
    return m_description;
}


