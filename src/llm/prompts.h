#ifndef PROMPTS_H
#define PROMPTS_H

#include <stdio.h>
#include <stdarg.h>

#define PROMPT_SECTION_COHERENCE \
    "[INST] Tu es un assistant academique expert en redaction " \
    "scientifique. Analyse le texte suivant et reponds " \
    "uniquement par OUI si le contenu est coherent et " \
    "complet, ou NON suivi d'une explication courte (max 2 " \
    "phrases) si ce n'est pas le cas.\n\n" \
    "Texte :\n%s\n\n" \
    "Critere : %s [/INST]"

#define PROMPT_REFORMULATE \
    "[INST] Reformule la phrase suivante pour qu'elle soit " \
    "plus academique et precise, sans en changer le sens. " \
    "Reponds uniquement avec la phrase reformulee, sans " \
    "explication.\n\nPhrase : %s [/INST]"

#define PROMPT_DETECT_SECTION \
    "[INST] Identifie le type de cette section de document " \
    "academique parmi : introduction, methodologie, resultats, " \
    "discussion, conclusion, references, autre.\n\n" \
    "Texte (debut) :\n%s\n\n" \
    "Reponds avec un seul mot. [/INST]"

#define PROMPT_CUSTOM_RULE \
    "[INST] %s\n\nTexte :\n%s\n\n" \
    "Reponds par OUI ou NON uniquement. [/INST]"

static inline int build_prompt(char *buf, size_t size,
                                const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return n;
}

#endif
