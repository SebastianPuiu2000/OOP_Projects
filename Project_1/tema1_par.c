#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "genetic_algorithm_par.h"

struct argument_struct {

	sack_object *objects;

	int object_count;
	int sack_capacity;
	int generations_count;
	int thread_id;
	int thread_count;

	pthread_barrier_t *barrier;

	individual *current_generation;
	individual *next_generation;

	int *sections_number;
	int *delimiters;

};

int main(int argc, char *argv[]) {

	struct argument_struct args;
	
	pthread_barrier_t barrier;


	// Citire date de input.
	if (!read_input(&args.objects, &args.object_count, &args.sack_capacity, &args.generations_count, &args.thread_count, argc, argv)) {
		return 0;
	}


	pthread_barrier_init(&barrier, NULL, args.thread_count);

	individual *current_generation = (individual*) calloc(args.object_count, sizeof(individual));
	individual *next_generation = (individual*) calloc(args.object_count, sizeof(individual));


	int *sections_number = calloc(1, sizeof(int));

	sections_number[0] = args.thread_count;

	int *delimiters = calloc(args.thread_count + 1, sizeof(int));

	delimiters[0] = 0;


	struct argument_struct arg[args.thread_count];

	// Initializarea tuturor argumentelor necesare thread-urilor.
	for(int i = 0; i < args.thread_count; i++) {

		arg[i].objects = args.objects;
		arg[i].object_count = args.object_count;
		arg[i].thread_count = args.thread_count;
		arg[i].sack_capacity = args.sack_capacity;
		arg[i].generations_count = args.generations_count;
		arg[i].thread_id = i;

		arg[i].current_generation = current_generation;
		arg[i].next_generation = next_generation;

		arg[i].barrier = &barrier;

		arg[i].sections_number = sections_number;
		arg[i].delimiters = delimiters;
	}

	

	pthread_t threads[args.thread_count];
	

	int r;
	// Pornire thread-uri. Atribuirea functiei si a argumentelor corespunzatoare.
	for (int i = 0; i < args.thread_count; i++) {

		r = pthread_create(&threads[i], NULL, run_genetic_algorithm, &arg[i]);

		if (r) {
            printf("Eroare la crearea thread-ului %d\n", i);
            exit(-1);
        }
	}

	// Oprire thread-uri.
	for (int i = 0; i < args.thread_count; i++) {

		r = pthread_join(threads[i], NULL);

		if (r) {
            printf("Eroare la asteptarea thread-ului %d\n", i);
            exit(-1);
        }
	}


	free(args.objects);

	pthread_barrier_destroy(&barrier);

	return 0;
}
