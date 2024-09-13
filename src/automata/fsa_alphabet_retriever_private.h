#pragma once

#include "alphabet_retriever_private.h"
#include "fsa_alphabet_retriever.h"

struct _FsaAlphabetRetriever {
	AlphabetRetriever *super;
};

void fsa_alphabet_retriever_destroy_private(AlphabetRetriever *alphabet_retriever);
GList *fsa_alphabet_retriever_get_alphabet_private(AlphabetRetriever *alphabet_retriever, Automaton *automaton);