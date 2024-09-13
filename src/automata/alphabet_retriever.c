#include "alphabet_retriever_private.h"

// Private function to initialize an alphabet retriever
AlphabetRetriever *alphabet_retriever_new_private()
{
	AlphabetRetriever *self = malloc(sizeof(AlphabetRetriever));
	self->get_alphabet = NULL;
	self->destroy = &alphabet_retriever_destroy_private;
	
	return self;
}

void alphabet_retriever_destroy_private(AlphabetRetriever *self)
{
	free(self);
}

void alphabet_retriever_destroy(AlphabetRetriever *self) 
{
	self->destroy(self);
}

GList *alphabet_retriever_get_alphabet(AlphabetRetriever *self, Automaton *automaton)
{
	return self->get_alphabet(self, automaton);
}