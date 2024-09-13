#include "fsa_alphabet_retriever_private.h"
#include "fsa_transition.h"

AlphabetRetriever *fsa_alphabet_retriever_new()
{
	AlphabetRetriever *super = alphabet_retriever_new_private();
	FsaAlphabetRetriever *self = malloc(sizeof(FsaAlphabetRetriever));
	
	super->derived = self;
	super->get_alphabet = &fsa_alphabet_retriever_get_alphabet_private;
	super->destroy = &fsa_alphabet_retriever_destroy_private;
	
	self->super = super;
	
	return super;
}

void fsa_alphabet_retriever_destroy_private(AlphabetRetriever *alphabet_retriever)
{
	FsaAlphabetRetriever *self = alphabet_retriever->derived;
	free(self);
	
	alphabet_retriever_destroy_private(alphabet_retriever);
}

GList *fsa_alphabet_retriever_get_alphabet_private(AlphabetRetriever *alphabet_retriever, Automaton *automaton)
{
	GHashTable *alphabet = g_hash_table_new((GHashFunc)g_str_hash, (GEqualFunc)g_str_equal);
	
	GList *transitions = automaton_get_transitions_as_list(automaton);
	
	GList *current_transition_node = transitions;
	for (GList *current_transition_node = transitions; current_transition_node != NULL; current_transition_node = current_transition_node->next) {
		Transition *t = (Transition *)current_transition_node->data;
		const char *symbol = fsa_transition_get_symbol(t);
		for (int j = 0; j < strlen(symbol); j++) {
			char *tmp = malloc(sizeof(char) * 2);
			*tmp = symbol[j];
			tmp[1] = '\0';
			g_hash_table_add(alphabet, (gpointer) tmp);
		}
	}
	
	g_list_free(transitions);
	
	GList *alphabet_to_return = g_hash_table_get_keys(alphabet);
	g_hash_table_destroy(alphabet);
	
	return alphabet_to_return;
}