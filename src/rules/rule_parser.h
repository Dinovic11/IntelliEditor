#ifndef INTELLIEDITOR_RULE_PARSER_H
#define INTELLIEDITOR_RULE_PARSER_H

#include "rule.h"

/*
 * ============================================================
 * RULE PARSER
 * ------------------------------------------------------------
 * Responsabilité :
 *  - Lire un fichier JSON de règles
 *  - Valider sa structure
 *  - Construire un RuleSet exploitable par le moteur
 *
 * Aucune logique métier ici.
 * Aucune évaluation des règles.
 * ============================================================
 */


/* ------------------------------------------------------------
 * Charge un fichier JSON de règles depuis le disque.
 *
 * @param path Chemin du fichier JSON
 * @return Pointeur vers un RuleSet alloué dynamiquement,
 *         ou NULL en cas d'erreur (format invalide, I/O, mémoire)
 * ------------------------------------------------------------
 */
RuleSet* rule_parser_load(const char *path);


/* ------------------------------------------------------------
 * Libère toute la mémoire allouée par le parser.
 *
 * @param ruleset RuleSet à libérer
 * ------------------------------------------------------------
 */
void rule_parser_free(RuleSet *ruleset);


/* ------------------------------------------------------------
 * Vérifie la validité structurelle d'une règle.
 * (champs obligatoires présents, types cohérents)
 *
 * Utilisée en interne par le parser, mais exposée
 * pour permettre des tests unitaires.
 *
 * @param rule Règle à vérifier
 * @return 1 si valide, 0 sinon
 * ------------------------------------------------------------
 */
int rule_parser_validate_rule(const Rule *rule);

#endif /* INTELLIEDITOR_RULE_PARSER_H */