typedef struct _AlphabetRetriever AlphabetRetriever;

#pragma once

// Public function to destroy alphabet retriever
void alphabet_retriever_destroy(AlphabetRetriever *self);
GList *alphabet_retriever_get_alphabet(AlphabetRetriever *self, Automaton *automaton);