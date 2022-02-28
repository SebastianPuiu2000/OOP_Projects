#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "genetic_algorithm_par.h"
#include <unistd.h>

#define mini(a, b) ((a) < (b)) ? (a) : (b)

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



int read_input(sack_object **objects, int *object_count, int *sack_capacity, int *generations_count, int *thread_count, int argc, char *argv[])
{
	FILE *fp;

	if (argc < 3) {
		fprintf(stderr, "Usage:\n\t./tema1 in_file generations_count\n");
		return 0;
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		return 0;
	}

	if (fscanf(fp, "%d %d", object_count, sack_capacity) < 2) {
		fclose(fp);
		return 0;
	}

	if (*object_count % 10) {
		fclose(fp);
		return 0;
	}

	sack_object *tmp_objects = (sack_object *) calloc(*object_count, sizeof(sack_object));

	for (int i = 0; i < *object_count; ++i) {
		if (fscanf(fp, "%d %d", &tmp_objects[i].profit, &tmp_objects[i].weight) < 2) {
			free(objects);
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	*generations_count = (int) strtol(argv[2], NULL, 10);
	
	if (*generations_count == 0) {
		free(tmp_objects);

		return 0;
	}

	*objects = tmp_objects;

	*thread_count = (int) strtol(argv[3], NULL, 10);


	return 1;
}

void print_objects(const sack_object *objects, int object_count)
{
	for (int i = 0; i < object_count; ++i) {
		printf("%d %d\n", objects[i].weight, objects[i].profit);
	}
}

void print_generation(const individual *generation, int limit)
{
	for (int i = 0; i < limit; ++i) {
		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			printf("%d ", generation[i].chromosomes[j]);
		}

		printf("\n%d - %d\n", i, generation[i].fitness);
	}
}

void print_best_fitness(const individual *generation)
{
	printf("%d\n", generation[0].fitness);
}



void compute_fitness_function(const sack_object *objects, individual *generation, int object_count, int sack_capacity, int thread_id, int thread_count)
{
	int weight;
	int profit;


	// Fiecare thread va aplica operatia "compute_fitness_function" in felul urmator: 
	// fiecare thread va avea alocata o portiune din generatia curenta, portiune in 
	// care tuturor indivizilor le va fi calculat si atribuit fitness-ul. 
	int start = thread_id * (double) object_count / thread_count;
	int end = mini((thread_id + 1) * (double) object_count / thread_count, object_count);

	for (int i = start; i < end; ++i) {

		weight = 0;
		profit = 0;

		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			if (generation[i].chromosomes[j]) {
				weight += objects[j].weight;
				profit += objects[j].profit;
			}
		}

		generation[i].fitness = (weight <= sack_capacity) ? profit : 0;
	}
}

int cmpfunc(const void *a, const void *b)
{
	int i;
	individual *first = (individual *) a;
	individual *second = (individual *) b;

	int res = second->fitness - first->fitness; // decreasing by fitness
	if (res == 0) {
		int first_count = 0, second_count = 0;

		for (i = 0; i < first->chromosome_length && i < second->chromosome_length; ++i) {
			first_count += first->chromosomes[i];
			second_count += second->chromosomes[i];
		}

		res = first_count - second_count; // increasing by number of objects in the sack
		if (res == 0) {
			return second->index - first->index;
		}
	}

	return res;
}


void mutate_bit_string_1(const individual *ind, int generation_index)
{
	int i, mutation_size;
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	if (ind->index % 2 == 0) {
		// for even-indexed individuals, mutate the first 40% chromosomes by a given step
		mutation_size = ind->chromosome_length * 4 / 10;
		for (i = 0; i < mutation_size; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	} else {
		// for odd-indexed individuals, mutate the last 80% chromosomes by a given step
		mutation_size = ind->chromosome_length * 8 / 10;
		for (i = ind->chromosome_length - mutation_size; i < ind->chromosome_length; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	}
}

void mutate_bit_string_2(const individual *ind, int generation_index)
{
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	// mutate all chromosomes by a given step
	for (int i = 0; i < ind->chromosome_length; i += step) {
		ind->chromosomes[i] = 1 - ind->chromosomes[i];
	}
}

void crossover(individual *parent1, individual *child1, int generation_index)
{
	individual *parent2 = parent1 + 1;
	individual *child2 = child1 + 1;
	int count = 1 + generation_index % parent1->chromosome_length;

	memcpy(child1->chromosomes, parent1->chromosomes, count * sizeof(int));
	memcpy(child1->chromosomes + count, parent2->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));

	memcpy(child2->chromosomes, parent2->chromosomes, count * sizeof(int));
	memcpy(child2->chromosomes + count, parent1->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));
}

void copy_individual(const individual *from, const individual *to)
{
	memcpy(to->chromosomes, from->chromosomes, from->chromosome_length * sizeof(int));
}

void free_generation(individual *generation)
{
	int i;

	for (i = 0; i < generation->chromosome_length; ++i) {
		free(generation[i].chromosomes);
		generation[i].chromosomes = NULL;
		generation[i].fitness = 0;
	}
}


// Functia prin care este realizata operatia de merge.
void merge_sections(individual *source, int start, int mid, int end, individual *destination) {
	int iA = start;
	int iB = mid;
	int i;

	// Realizarea operatiei de merge conform criteriilor din functia cmpfunc.
	for (i = start; i < end; i++) {
		if (end == iB || (iA < mid && cmpfunc(source + iA, source + iB) < 0)) {
			destination[i] = source[iA];
			iA++;
		} else {
			destination[i] = source[iB];
			iB++;
		}
	}
}


// Functia prin care vor fi realizate toate operatiile de merge.
void merge_all_sections(individual *generation, int thread_id, int *sections_number, int *delimiters, pthread_barrier_t *barrier) {

	// Trebuie sa existe cel putin doua sectiuni pentru a se face operatia de merge.
	while((*sections_number) >= 2) {

			if((*sections_number) % 2 == 0) {

				if(thread_id % 2 == 0 && thread_id < (*sections_number)) {

					// Extragere indexi necesari pentru a face operatia de merge
					int start = delimiters[thread_id];
					int mid = delimiters[thread_id + 1];
					int end = delimiters[thread_id + 2];

					// Realizare operatie de merge in sectiunile corespunzatoare din generatia curenta
					individual* merged_section = calloc(end - start, sizeof(individual));
    				merge_sections(generation + start, 0, mid - start, end - start, merged_section);
    				memcpy(generation + start, merged_section, (end - start) * sizeof(individual));
    				free(merged_section);
				}
			}

			if((*sections_number) % 2 == 1) {

				if(thread_id % 2 == 0 && thread_id < (*sections_number) - 1) {

					// Extragere indexi necesari pentru a face operatia de merge
					int start = delimiters[thread_id];
					int mid = delimiters[thread_id + 1];
					int end = delimiters[thread_id + 2];

					// Realizare operatie de merge in sectiunile corespunzatoare din generatia curenta
					individual* merged_section = calloc(end - start, sizeof(individual));
    				merge_sections(generation + start, 0, mid - start, end - start, merged_section);
    				memcpy(generation + start, merged_section, (end - start) * sizeof(individual));
    				free(merged_section);
				}
			}

			pthread_barrier_wait(barrier);

			// Thread-ul cu id-ul 0 este responsabil de reactualizarea delimitatorilor.
			// De asemenea, este responsabil si de reactualizarea numarului de sectiuni.
			if(thread_id == 0) {

				if((*sections_number) % 2 == 0) {

					// Actualizare delimitatori
					for(int i = 1; i < (*sections_number); i++) {

						int stop = 0;
						for(int j = i; j < (*sections_number) && !stop; j++) {

							if(delimiters[j] == delimiters[j+1]) {
								stop = 1;
							}

							delimiters[j] = delimiters[j+1];
						}
					}

					// Actualizare numar de sectiuni
					int aux = (*sections_number);
					aux = aux / 2;
					(*sections_number) = aux;
				}

				if((*sections_number) % 2 == 1) {

					// Actualizare delimitatori
					for(int i = 1; i < (*sections_number) - 1; i++) {

						int stop = 0;
						for(int j = i; j < (*sections_number) && !stop; j++) {

							if(delimiters[j] == delimiters[j+1]) {
								stop = 1;
							}

							delimiters[j] = delimiters[j+1];
						}
					}

					// Actualizare numar de sectiuni
					int aux = (*sections_number);
					aux = aux / 2 + 1;
					(*sections_number) = aux;

				}

			}


			pthread_barrier_wait(barrier);
		}
}



void *run_genetic_algorithm(void *args) {

	// Transmiterea argumentelor primite.
	struct argument_struct *arguments = (struct argument_struct *)args;

    int count, cursor;
   
	individual *tmp = NULL;


	// Fiecarui thread ii va fi alocat o portiune din generatia curenta.
	// Fiecare thread va fi responsabil de initializarea indivizilor din sectiunea asociata lui.
	int start = arguments->thread_id * (double)arguments->object_count / arguments->thread_count;
	int end = mini((arguments->thread_id + 1) * (double)arguments->object_count / arguments->thread_count, arguments->object_count);
	
	for (int i = start; i < end; ++i) {
		arguments->current_generation[i].fitness = 0;
		arguments->current_generation[i].chromosomes = (int*) calloc(arguments->object_count, sizeof(int));
		arguments->current_generation[i].chromosomes[i] = 1;
		arguments->current_generation[i].index = i;
		arguments->current_generation[i].chromosome_length = arguments->object_count;

		arguments->next_generation[i].fitness = 0;
		arguments->next_generation[i].chromosomes = (int*) calloc(arguments->object_count, sizeof(int));
		arguments->next_generation[i].index = i;
		arguments->next_generation[i].chromosome_length = arguments->object_count;
	}


	pthread_barrier_wait(arguments->barrier);


	// Toate thread-urile vor itera prin fiecare generatie.
	for (int k = 0; k < arguments->generations_count; ++k) {
		
		//pthread_barrier_wait(arguments->barrier);

		cursor = 0;		
		
		// Fiecare thread va aplica operatia "compute_fitness_function" in felul urmator: 
		// fiecare thread va avea alocata o portiune din generatia curenta, portiune in 
		// care tuturor indivizilor le va fi calculat si atribuit fitness-ul. 
		compute_fitness_function(arguments->objects, arguments->current_generation, arguments->object_count, arguments->sack_capacity, arguments->thread_id, arguments->thread_count);	

		pthread_barrier_wait(arguments->barrier);


		start = arguments->thread_id * (double)arguments->object_count / arguments->thread_count;
		end = mini((arguments->thread_id + 1) * (double)arguments->object_count / arguments->thread_count, arguments->object_count);		

		// Se construieste un spatiu comun pentru toate thread-urile existente.
		// Exista zona de delimitatori, cu ajutorul careia fiecare thread o sa 
		// stie care sunt "granitele" intre care sa faca operatia de merge.
		// Exista zona de sectiuni, cu ajutorul careia fiecare thread stie cate 
		// sectiuni care necesita sa le fie aplicate operatia de merge exista. 
		arguments->delimiters[arguments->thread_id + 1] = end;
		(*arguments->sections_number) = arguments->thread_count;

		// Fiecare thread va aplica operatia "qsort" pe portiunea din generatia curenta 
		// ce ii este alocata. Astfel, se vor obtiune mai multe sectiuni in generatia 
		// curenta, ce sunt sortate local.
		qsort(arguments->current_generation + start, end - start, sizeof(individual), cmpfunc);


		pthread_barrier_wait(arguments->barrier);


		// Toate sectiunile sortate ce au fost obtinute la pasul anterior trebuie sa 
		// fie supuse proceselor de imbinare si ordonare (sa fie "merge-uite").
		merge_all_sections(arguments->current_generation, arguments->thread_id, arguments->sections_number, arguments->delimiters, arguments->barrier);
		

		pthread_barrier_wait(arguments->barrier);
		


		// Fiecarui thread ii va fi alocat o portiune din primii 30% copii din generatia curenta.
		// Fiecare thread va realiza operatia "copy_individual", prin care portiunea alocata lui 
		// din generatia curenta va fi copiata in generatia urmatoare.
		count = arguments->object_count * 3 / 10;

		start = arguments->thread_id * (double) count / arguments->thread_count;
		end = mini((arguments->thread_id + 1) * (double) count / arguments->thread_count, count);

		for (int i = start; i < end; ++i) {
			copy_individual(arguments->current_generation + i, arguments->next_generation + i);
		}
		cursor = count;


		
		// Fiecarui thread ii va fi alocat o portiune din primii 20% indivizi din generatia curenta.
		// Fiecare thread va aplica operatia "mutate_bit_string_1" asupra portiunii alocate lui.
		count = arguments->object_count * 2 / 10;

		start = arguments->thread_id * (double) count / arguments->thread_count;
		end = mini((arguments->thread_id + 1) * (double) count / arguments->thread_count, count);

		for (int i = start; i < end; ++i) {
			copy_individual(arguments->current_generation + i, arguments->next_generation + cursor + i);
			mutate_bit_string_1(arguments->next_generation + cursor + i, k);
		}
		cursor += count;
	


		// Fiecarui thread ii va fi alocat o portiune din urmatorii 20% indivizi din gereratia curenta.
		// Fiecare thread va aplica operatia "mutate_bit_string_2" asupra portiunii alocate lui.
		count = arguments->object_count * 2 / 10;

		start = arguments->thread_id * (double) count / arguments->thread_count;
		end = mini((arguments->thread_id + 1) * (double) count / arguments->thread_count, count);


		for (int i = start; i < end; ++i) {
			copy_individual(arguments->current_generation + i + count, arguments->next_generation + cursor + i);
			mutate_bit_string_2(arguments->next_generation + cursor + i, k);
		}
		cursor += count;



		// Fiecarui thread ii va fi alocat o portiune din primii 30% indivizi din generatia curenta.
		count = arguments->object_count * 3 / 10;

		// Daca exista un numar impar de parinti, atunci ultimul va fi pastrat.
		// Thread-ul cu id-ul 0 este responsabil de copierea lui in urmatoare 
		// generatie. Dupa aceasta operatie, fiecare thread isi actualizeaza count.
		if (count % 2 == 1) {

			if(arguments->thread_id == 0)
				copy_individual(arguments->current_generation + arguments->object_count - 1, arguments->next_generation + cursor + count - 1);

			count--;
		}
		

		start = arguments->thread_id * (double) count / arguments->thread_count;
		end = mini((arguments->thread_id + 1) * (double) count / arguments->thread_count, count);


		// Fiecare thread va aplica operatia "crossover" pe portiunea lui cu parinti doi cate doi.
		for (int i = start; i < end; i++) {
			if(i % 2 == 0)
				crossover(arguments->current_generation + i, arguments->next_generation + cursor + i, k);
		}



		pthread_barrier_wait(arguments->barrier);
		
		// Schimbare pointeri intre generatia curenta si cea viitoare pentru fiecare thread.
		tmp = arguments->current_generation;
		arguments->current_generation = arguments->next_generation;
		arguments->next_generation = tmp;
		
		pthread_barrier_wait(arguments->barrier);


		// Fiecarui thread ii va fi alocat o portiune din generatia curenta. 
		// Fiecare thread va fi responsabil de reindexarea indivizilor ce 
		// fac parte din sectiunea asociata lui.
		start = arguments->thread_id * (double) arguments->object_count / arguments->thread_count;
		end = mini((arguments->thread_id + 1) * (double) arguments->object_count / arguments->thread_count, arguments->object_count);

		
		for (int i = start; i < end; ++i) {
			arguments->current_generation[i].index = i;
		}
		

		// Thread-ul cu id-ul 0 este responsabil cu afisarea celui mai bun fitness din generatia curenta.
		// Generatia curenta trebuie sa fie multiplu de 5.
		if (k % 5 == 0 && arguments->thread_id == 0) {
			print_best_fitness(arguments->current_generation);
		}

		pthread_barrier_wait(arguments->barrier);
	}




	pthread_barrier_wait(arguments->barrier);

	// Fiecare thread va aplica operatia "compute_fitness_function" in felul urmator: 
	// fiecare thread va avea alocata o portiune din generatia curenta, portiune in 
	// care tuturor indivizilor le va fi calculat si atribuit fitness-ul.
	compute_fitness_function(arguments->objects, arguments->current_generation, arguments->object_count, arguments->sack_capacity, arguments->thread_id, arguments->thread_count);	

	pthread_barrier_wait(arguments->barrier);


	start = arguments->thread_id * (double)arguments->object_count / arguments->thread_count;
	end = mini((arguments->thread_id + 1) * (double)arguments->object_count / arguments->thread_count, arguments->object_count);

	// Se construieste un spatiu comun pentru toate thread-urile existente.
	// Exista zona de delimitatori, cu ajutorul careia fiecare thread o sa 
	// stie care sunt "granitele" intre care sa faca operatia de merge.
	// Exista zona de sectiuni, cu ajutorul careia fiecare thread stie cate 
	// sectiuni exista care necesita sa le fie aplicate operatia de merge. 
	arguments->delimiters[arguments->thread_id + 1] = end;
	(*arguments->sections_number) = arguments->thread_count;


	// Fiecare thread va aplica operatia "qsort" pe portiunea din generatia curenta 
	// ce ii este alocata. Astfel, se vor obtiune mai multe sectiuni in generatia 
	// curenta, ce sunt sortate local.
	qsort(arguments->current_generation + start, end - start, sizeof(individual), cmpfunc);

	
	pthread_barrier_wait(arguments->barrier);


	// Toate sectiunile sortate ce au fost obtinute la pasul anterior trebuie sa 
	// fie supuse proceselor de imbinare si ordonare (sa fie "merge-uite").
	merge_all_sections(arguments->current_generation, arguments->thread_id, arguments->sections_number, arguments->delimiters, arguments->barrier);
	

	pthread_barrier_wait(arguments->barrier);


	// Thread-ul cu id-ul 0 este responsabil cu afisarea celui mai bun fitness din generatia curenta.
	if(arguments->thread_id == 0) {
		print_best_fitness(arguments->current_generation);
	}


	// Thread-ul cu id-ul 0 este responsabil de dezalocarea memoriei.
	if(arguments->thread_id == 0) {

		// Dezalocare memorie generatii
		free_generation(arguments->current_generation);
		free_generation(arguments->next_generation);

		// Dezalocare memorie resurse
		free(arguments->current_generation);
		free(arguments->next_generation);
	}

	pthread_exit(NULL);
}

