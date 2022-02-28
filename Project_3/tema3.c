#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// functie ce construieste un vector, care contine toata informatia despre topologie
void compute_send_data_array(int *clusters_info, 
                             int *cluster_0_workers, 
                             int cluster_0_num_workers, 
                             int *cluster_1_workers, 
                             int cluster_1_num_workers, 
                             int *cluster_2_workers, 
                             int cluster_2_num_workers) {

    int k = 0;
    int j = 0;

    clusters_info[k] = cluster_0_num_workers;

    k++;

    while(j < cluster_0_num_workers) {
        clusters_info[k] = cluster_0_workers[j];
        k++;
        j++;
    }

    clusters_info[k] = cluster_1_num_workers;

    j = 0;
    k++;

    while(j < cluster_1_num_workers) {
        clusters_info[k] = cluster_1_workers[j];
        k++;
        j++;
    }

    clusters_info[k] = cluster_2_num_workers;

    j = 0;
    k++;

    while(j < cluster_2_num_workers) {
        clusters_info[k] = cluster_2_workers[j];
        k++;
        j++;
    }

}

// functie comparator necesara pentru qsort
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

// functie ce afiseaza topologia sistemului de procese
void display_topology(int rank, int *cluster_0_workers, int cluster_0_num_workers, int *cluster_1_workers, int cluster_1_num_workers, int *cluster_2_workers, int cluster_2_num_workers) {

    printf("%d -> ", rank);

        printf("0:");
        for(int i = 0; i < cluster_0_num_workers; i++) {
            printf("%d", cluster_0_workers[i]);

            if(i != cluster_0_num_workers - 1) {
                printf(",");
            }
            else {
                printf(" ");
            }
        }

        printf("1:");
        for(int i = 0; i < cluster_1_num_workers; i++) {
            printf("%d", cluster_1_workers[i]);

            if(i != cluster_1_num_workers - 1) {
                printf(",");
            }
            else {
                printf(" ");
            }
        }

        printf("2:");
        for(int i = 0; i < cluster_2_num_workers; i++) {
            printf("%d", cluster_2_workers[i]);

            if(i != cluster_2_num_workers - 1) {
                printf(",");
            }
            else {
                printf(" ");
            }
        }

        printf("\n");
}

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    FILE *f;

    int *cluster_0_workers;
    int *cluster_1_workers;
    int *cluster_2_workers;

    int cluster_0_num_workers = 0;
    int cluster_1_num_workers = 0;
    int cluster_2_num_workers = 0;

    int cluster_0_id = 0;
    int cluster_1_id = 1;
    int cluster_2_id = 2;    

    int *clusters_info;
    int clusters_info_size;

    int tasks_number;
    int *tasks;
    int undistributed_tasks;
    int sent_tasks;
    int received_tasks;

    int communication_error;

    int cluster_0_num_tasks;
    int cluster_1_num_tasks;
    int cluster_2_num_tasks;

    int worker_num_tasks;

    int cluster_coordinator;

    if (rank == 0) {

        // citirea informatiilor din fisierul asociat acestui proces coordonator
        f = fopen("cluster0.txt", "r");

        fscanf(f, "%d", &cluster_0_num_workers);
        cluster_0_workers = calloc(cluster_0_num_workers, sizeof(int));


        for(int i = 0; i < cluster_0_num_workers; i++) {
            fscanf(f, "%d", &cluster_0_workers[i]);
        }


        MPI_Status status;


        // se trimite numarul de workeri din cluster-ul curent
        printf("M(%d,%d)\n", rank, 1);
        MPI_Send(&cluster_0_num_workers, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&cluster_0_num_workers, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);


        // se primeste numarul de workeri de la celelalte 2 clustere
        MPI_Recv(&cluster_1_num_workers, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster_2_num_workers, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);


        cluster_1_workers = calloc(cluster_1_num_workers, sizeof(int));
        cluster_2_workers = calloc(cluster_2_num_workers, sizeof(int));


        // se trimite lista de workeri din cluster-ul curent 
        printf("M(%d,%d)\n", rank, 1);
        MPI_Send(&cluster_0_workers[0], cluster_0_num_workers, MPI_INT, 1, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&cluster_0_workers[0], cluster_0_num_workers, MPI_INT, 2, 0, MPI_COMM_WORLD);


        // se primeste lista de workeri de la celelalte 2 clustere
        MPI_Recv(&cluster_1_workers[0], cluster_1_num_workers, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster_2_workers[0], cluster_2_num_workers, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);


        // este afisata topologia obtinuta
        display_topology(rank, cluster_0_workers, cluster_0_num_workers, cluster_1_workers, cluster_1_num_workers, cluster_2_workers, cluster_2_num_workers);


        clusters_info_size = 1 + cluster_0_num_workers + 1 + cluster_1_num_workers + 1 + cluster_2_num_workers;
        clusters_info = calloc(clusters_info_size, sizeof(int));

        // este obtinut un vector ce contine informatia despre topologia intregului sistem de procese
        // acesta este compus din numarul de workeri din clusterul 0, urmat de workerii din 
        // clusterul 0, urmat de numarul de workeri din clusterul 1, urmat de workerii din 
        // clusterul 1, urmat de numarul de workeri din clusterului 2, urmat de workerii din 
        // clusterul 2
        compute_send_data_array(clusters_info, cluster_0_workers, cluster_0_num_workers, cluster_1_workers, cluster_1_num_workers, cluster_2_workers, cluster_2_num_workers);

        for(int i = 0; i < cluster_0_num_workers; i++) {

            // workerul curent este informat in legatura cu cine este coordonatorul sau
            printf("M(%d,%d)\n", rank, cluster_0_workers[i]);
            MPI_Send(&rank, 1, MPI_INT, cluster_0_workers[i], 0, MPI_COMM_WORLD);

            // se trimite informatia referitoare la topologie, ce a fost obtinuta anterior
            printf("M(%d,%d)\n", rank, cluster_0_workers[i]);
            MPI_Send(&clusters_info_size, 1, MPI_INT, cluster_0_workers[i], 0, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", rank, cluster_0_workers[i]);
            MPI_Send(&clusters_info[0], clusters_info_size, MPI_INT, cluster_0_workers[i], 0, MPI_COMM_WORLD);

        }



        // parsarea argumentelor
        tasks_number = atoi(argv[1]);
        communication_error = atoi(argv[2]);


        // construirea vectorului cu elementele ce urmeaza sa fie dublate (vector de taskuri)
        tasks = calloc(tasks_number, sizeof(int));

        for(int i = 0; i < tasks_number; i++) {
            tasks[i] = i;
        }

        // se va asocia pentru fiecare cluster un numar proportional de task-uri cu numarul de workeri
        int number_all_workers = cluster_0_num_workers + cluster_1_num_workers + cluster_2_num_workers;

        cluster_0_num_tasks = (tasks_number / number_all_workers) * cluster_0_num_workers;

        cluster_1_num_tasks = (tasks_number / number_all_workers) * cluster_1_num_workers;

        cluster_2_num_tasks = (tasks_number / number_all_workers) * cluster_2_num_workers;


        // exista sansa sa ramana task-uri nedistribuite daca numarul 
        // de task-uri nu se imparte exact la numarul de workeri
        int m = 0, n = 0;
        undistributed_tasks = tasks_number % number_all_workers;

        // task-urile ramase vor fi distribuite in mod cat mai egal 
        while(m < undistributed_tasks) {

            n = 0;
            while(m < undistributed_tasks && n < cluster_0_num_workers) {
                m++;
                cluster_0_num_tasks++;
            }

            n = 0;
            while(m < undistributed_tasks && n < cluster_1_num_workers) {
                m++;
                cluster_1_num_tasks++;
            }

            n = 0;
            while(m < undistributed_tasks && n < cluster_2_num_workers) {
                m++;
                cluster_2_num_tasks++;
            }
        }

      
        // se trimite numarul de task-uri aferent fiecarui cluster
        printf("M(%d,%d)\n", rank, 1);
        MPI_Send(&cluster_1_num_tasks, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&cluster_2_num_tasks, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);


        // se trimit task-urile aferente fiecarui cluster
        printf("M(%d,%d)\n", rank, 1);
        MPI_Send(&tasks[cluster_0_num_tasks], cluster_1_num_tasks, MPI_INT, 1, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&tasks[cluster_0_num_tasks + cluster_1_num_tasks], cluster_2_num_tasks, MPI_INT, 2, 0, MPI_COMM_WORLD);


        // se impart task-urile in mod egal intre workeri
        worker_num_tasks = cluster_0_num_tasks / cluster_0_num_workers;

        // se calculeaza numarul de task-uri nedistribuite
        undistributed_tasks = cluster_0_num_tasks % cluster_0_num_workers;
        
        // se parcurg toti workerii din clusterul curent
        sent_tasks = 0;
        for(int i = 0; i < cluster_0_num_workers; i++) {

            // numarul de task-uri asociat workerului curent
            worker_num_tasks = cluster_0_num_tasks / cluster_0_num_workers;

            // daca mai exista task-uri nedistribuite, workerul curent va primi unul in plus
            if(undistributed_tasks > 0) {
                undistributed_tasks--;
                worker_num_tasks++;
            }

            // se trimite informatia cu numarul de task-uri catre workerul curent
            printf("M(%d,%d)\n", rank, cluster_0_workers[i]);
            MPI_Send(&worker_num_tasks, 1, MPI_INT, cluster_0_workers[i], 0, MPI_COMM_WORLD);

            // se trimite informatia cu vectorul de task-uri catre workerul curent
            printf("M(%d,%d)\n", rank, cluster_0_workers[i]);
            MPI_Send(&tasks[0 + sent_tasks], worker_num_tasks, MPI_INT, cluster_0_workers[i], 0, MPI_COMM_WORLD);

            sent_tasks += worker_num_tasks;
        }

        undistributed_tasks = cluster_0_num_tasks % cluster_0_num_workers;

        // se procedeaza similar ca in pasul anterioar, doar ca de
        // aceasta data se primesc rezultatele obtinute de workeri
        received_tasks = 0;
        for(int i = 0; i < cluster_0_num_workers; i++) {

            worker_num_tasks = cluster_0_num_tasks / cluster_0_num_workers;

            if(undistributed_tasks > 0) {
                undistributed_tasks--;
                worker_num_tasks++;
            }

         
            MPI_Recv(&tasks[0 + received_tasks], worker_num_tasks, MPI_INT, cluster_0_workers[i], 0, MPI_COMM_WORLD, &status);

            received_tasks += worker_num_tasks;
        }

        // se primesc rezultatele de la ceilalti coordonatori
        MPI_Recv(&tasks[cluster_0_num_tasks], cluster_1_num_tasks, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&tasks[cluster_0_num_tasks + cluster_1_num_tasks], cluster_2_num_tasks, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);

        // rezultatele sunt sortate in ordine crescatoare
        qsort(tasks, tasks_number, sizeof(int), cmpfunc);

        // este afisat rezultatul final
        printf("Rezultat: ");
        for(int i = 0; i < tasks_number; i++) {
            printf("%d ", tasks[i]);
        }
        printf("\n");
    }

    if (rank == 1) {

        // citirea informatiilor din fisierul asociat acestui proces coordonator
        f = fopen("cluster1.txt", "r");

        fscanf(f, "%d", &cluster_1_num_workers);
        cluster_1_workers = calloc(cluster_1_num_workers, sizeof(int)); 


        for(int i = 0; i < cluster_1_num_workers; i++) {
            fscanf(f, "%d", &cluster_1_workers[i]);
        }


        MPI_Status status;


        // se trimite numarul de workeri din cluster-ul curent
        printf("M(%d,%d)\n", rank, 0);
        MPI_Send(&cluster_1_num_workers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&cluster_1_num_workers, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);


        // se primeste numarul de workeri de la celelalte 2 clustere
        MPI_Recv(&cluster_0_num_workers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster_2_num_workers, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);


        cluster_0_workers = calloc(cluster_0_num_workers, sizeof(int)); 
        cluster_2_workers = calloc(cluster_2_num_workers, sizeof(int)); 


        // se trimite lista de workeri din cluster-ul curent 
        printf("M(%d,%d)\n", rank, 0);
        MPI_Send(&cluster_1_workers[0], cluster_1_num_workers, MPI_INT, 0, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 2);
        MPI_Send(&cluster_1_workers[0], cluster_1_num_workers, MPI_INT, 2, 0, MPI_COMM_WORLD);


        // se primeste lista de workeri de la celelalte 2 clustere
        MPI_Recv(&cluster_0_workers[0], cluster_0_num_workers, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster_2_workers[0], cluster_2_num_workers, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);


        // este afisata topologia obtinuta
        display_topology(rank, cluster_0_workers, cluster_0_num_workers, cluster_1_workers, cluster_1_num_workers, cluster_2_workers, cluster_2_num_workers);


        clusters_info_size = 1 + cluster_0_num_workers + 1 + cluster_1_num_workers + 1 + cluster_2_num_workers;
        clusters_info = calloc(clusters_info_size, sizeof(int));

        // este obtinut un vector ce contine informatia despre topologia intregului sistem de procese
        // acesta este compus din numarul de workeri din clusterul 0, urmat de workerii din 
        // clusterul 0, urmat de numarul de workeri din clusterul 1, urmat de workerii din 
        // clusterul 1, urmat de numarul de workeri din clusterului 2, urmat de workerii din 
        // clusterul 2
        compute_send_data_array(clusters_info, cluster_0_workers, cluster_0_num_workers, cluster_1_workers, cluster_1_num_workers, cluster_2_workers, cluster_2_num_workers);

        // se parcurg toti workerii din clusterul curent
        for(int i = 0; i < cluster_1_num_workers; i++) {

            // workerul curent este informat in legatura cu cine este coordonatorul sau
            printf("M(%d,%d)\n", rank, cluster_1_workers[i]);
            MPI_Send(&rank, 1, MPI_INT, cluster_1_workers[i], 0, MPI_COMM_WORLD);

            // se trimite informatia referitoare la topologie, ce a fost obtinuta anterior
            printf("M(%d,%d)\n", rank, cluster_1_workers[i]);
            MPI_Send(&clusters_info_size, 1, MPI_INT, cluster_1_workers[i], 0, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", rank, cluster_1_workers[i]);
            MPI_Send(&clusters_info[0], clusters_info_size, MPI_INT, cluster_1_workers[i], 0, MPI_COMM_WORLD);

        }



        // este primit numarul de task-uri repartizate de catre procesul 0 pentru clusterul curent
        MPI_Recv(&cluster_1_num_tasks, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        tasks = calloc(cluster_1_num_tasks, sizeof(int));

        // sunt primite task-urile repartizate de catre procesul 0 pentru clusterul curent
        MPI_Recv(&tasks[0], cluster_1_num_tasks, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        // se impart task-urile in mod egal intre workeri
        worker_num_tasks = cluster_1_num_tasks / cluster_1_num_workers;

        // se calculeaza numarul de task-uri nedistribuite
        undistributed_tasks = cluster_1_num_tasks % cluster_1_num_workers;

        // se parcurg toti workerii din clusterul curent
        sent_tasks = 0;
        for(int i = 0; i < cluster_1_num_workers; i++) {

            // numarul de task-uri asociat workerului curent
            worker_num_tasks = cluster_1_num_tasks / cluster_1_num_workers;

            // daca mai exista task-uri nedistribuite, workerul curent va primi unul in plus
            if(undistributed_tasks > 0) {
               undistributed_tasks--;
               worker_num_tasks++;
            }

            // se trimite informatia cu numarul de task-uri catre workerul curent
            printf("M(%d,%d)\n", rank, cluster_1_workers[i]);
            MPI_Send(&worker_num_tasks, 1, MPI_INT, cluster_1_workers[i], 0, MPI_COMM_WORLD);

            // se trimite informatia cu vectorul de task-uri catre workerul curent
            printf("M(%d,%d)\n", rank, cluster_1_workers[i]);
            MPI_Send(&tasks[0 + sent_tasks], worker_num_tasks, MPI_INT, cluster_1_workers[i], 0, MPI_COMM_WORLD);

            sent_tasks += worker_num_tasks;
        }

        undistributed_tasks = cluster_1_num_tasks % cluster_1_num_workers;

        // se procedeaza similar ca in pasul anterioar, doar ca de
        // aceasta data se primesc rezultatele obtinute de workeri
        received_tasks = 0;
        for(int i = 0; i < cluster_1_num_workers; i++) {

            worker_num_tasks = cluster_1_num_tasks / cluster_1_num_workers;

            if(undistributed_tasks > 0) {
               undistributed_tasks--;
               worker_num_tasks++;
            }


            MPI_Recv(&tasks[0 + received_tasks], worker_num_tasks, MPI_INT, cluster_1_workers[i], 0, MPI_COMM_WORLD, &status);

            received_tasks += worker_num_tasks;
        }

        // toatele rezultatele obtinute de fiecare worker din clusterul curent sunt trimise catre procesul 0
        printf("M(%d,%d)\n", rank, 0);
        MPI_Send(&tasks[0], cluster_1_num_tasks, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }

    if (rank == 2) {

        // citirea informatiilor din fisierul asociat acestui proces coordonator
        f = fopen("cluster2.txt", "r");

        fscanf(f, "%d", &cluster_2_num_workers);
        cluster_2_workers = calloc(cluster_2_num_workers, sizeof(int)); 


        for(int i = 0; i < cluster_2_num_workers; i++) {
            fscanf(f, "%d", &cluster_2_workers[i]);
        }


        MPI_Status status;

        // se trimite numarul de workeri din cluster-ul curent
        printf("M(%d,%d)\n", rank, 0);
        MPI_Send(&cluster_2_num_workers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 1);
        MPI_Send(&cluster_2_num_workers, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);


        // se primeste numarul de workeri de la celelalte 2 clustere
        MPI_Recv(&cluster_0_num_workers, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster_1_num_workers, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);

        
        cluster_0_workers = calloc(cluster_0_num_workers, sizeof(int)); 
        cluster_1_workers = calloc(cluster_1_num_workers, sizeof(int));


        // se trimite lista de workeri din cluster-ul curent 
        printf("M(%d,%d)\n", rank, 0);
        MPI_Send(&cluster_2_workers[0], cluster_2_num_workers, MPI_INT, 0, 0, MPI_COMM_WORLD);

        printf("M(%d,%d)\n", rank, 1);
        MPI_Send(&cluster_2_workers[0], cluster_2_num_workers, MPI_INT, 1, 0, MPI_COMM_WORLD);


        // se primeste lista de workeri de la celelalte 2 clustere
        MPI_Recv(&cluster_0_workers[0], cluster_0_num_workers, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&cluster_1_workers[0], cluster_1_num_workers, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);


        // este afisata topologia obtinuta
        display_topology(rank, cluster_0_workers, cluster_0_num_workers, cluster_1_workers, cluster_1_num_workers, cluster_2_workers, cluster_2_num_workers);


        clusters_info_size = 1 + cluster_0_num_workers + 1 + cluster_1_num_workers + 1 + cluster_2_num_workers;
        clusters_info = calloc(clusters_info_size, sizeof(int));

        // este obtinut un vector ce contine informatia despre topologia intregului sistem de procese
        // acesta este compus din numarul de workeri din clusterul 0, urmat de workerii din 
        // clusterul 0, urmat de numarul de workeri din clusterul 1, urmat de workerii din 
        // clusterul 1, urmat de numarul de workeri din clusterului 2, urmat de workerii din 
        // clusterul 2
        compute_send_data_array(clusters_info, cluster_0_workers, cluster_0_num_workers, cluster_1_workers, cluster_1_num_workers, cluster_2_workers, cluster_2_num_workers);
 
        for(int i = 0; i < cluster_2_num_workers; i++) {

            // workerul curent este informat in legatura cu cine este coordonatorul sau
            printf("M(%d,%d)\n", rank, cluster_2_workers[i]);
            MPI_Send(&rank, 1, MPI_INT, cluster_2_workers[i], 0, MPI_COMM_WORLD);

            // se trimite informatia referitoare la topologie, ce a fost obtinuta anterior
            printf("M(%d,%d)\n", rank, cluster_2_workers[i]);
            MPI_Send(&clusters_info_size, 1, MPI_INT, cluster_2_workers[i], 0, MPI_COMM_WORLD);

            printf("M(%d,%d)\n", rank, cluster_2_workers[i]);
            MPI_Send(&clusters_info[0], clusters_info_size, MPI_INT, cluster_2_workers[i], 0, MPI_COMM_WORLD);
        }



        // este primit numarul de task-uri repartizate de catre procesul 0 pentru clusterul curent
        MPI_Recv(&cluster_2_num_tasks, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        tasks = calloc(cluster_2_num_tasks, sizeof(int));

        // sunt primite task-urile repartizate de catre procesul 0 pentru clusterul curent
        MPI_Recv(&tasks[0], cluster_2_num_tasks, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);


        // se impart task-urile in mod egal intre workeri
        worker_num_tasks = cluster_2_num_tasks / cluster_2_num_workers;

        // se calculeaza numarul de task-uri nedistribuite
        undistributed_tasks = cluster_2_num_tasks % cluster_2_num_workers;

        // se parcurg toti workerii din clusterul curent
        sent_tasks = 0;
        for(int i = 0; i < cluster_2_num_workers; i++) {

            // numarul de task-uri asociat workerului curent
            worker_num_tasks = cluster_2_num_tasks / cluster_2_num_workers;

            // daca mai exista task-uri nedistribuite, workerul curent va primi unul in plus
            if(undistributed_tasks > 0) {
               undistributed_tasks--;
               worker_num_tasks++;
            }

            // se trimite informatia cu numarul de task-uri catre workerul curent
            printf("M(%d,%d)\n", rank, cluster_2_workers[i]);
            MPI_Send(&worker_num_tasks, 1, MPI_INT, cluster_2_workers[i], 0, MPI_COMM_WORLD);

            // se trimite informatia cu vectorul de task-uri catre workerul curent
            printf("M(%d,%d)\n", rank, cluster_2_workers[i]);
            MPI_Send(&tasks[0 + sent_tasks], worker_num_tasks, MPI_INT, cluster_2_workers[i], 0, MPI_COMM_WORLD);

            sent_tasks += worker_num_tasks;
        } 

        undistributed_tasks = cluster_2_num_tasks % cluster_2_num_workers;

        // se procedeaza similar ca in pasul anterioar, doar ca de
        // aceasta data se primesc rezultatele obtinute de workeri
        received_tasks = 0;
        for(int i = 0; i < cluster_2_num_workers; i++) {

            worker_num_tasks = cluster_2_num_tasks / cluster_2_num_workers;

            if(undistributed_tasks > 0) {
               undistributed_tasks--;
               worker_num_tasks++;
            }


            MPI_Recv(&tasks[0 + received_tasks], worker_num_tasks, MPI_INT, cluster_2_workers[i], 0, MPI_COMM_WORLD, &status);

            received_tasks += worker_num_tasks;
        } 

        // toatele rezultatele obtinute de fiecare worker din clusterul curent sunt trimise catre procesul 0
        printf("M(%d,%d)\n", rank, 0);
        MPI_Send(&tasks[0], cluster_2_num_tasks, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }


    if(rank > 2) {
        
        MPI_Status status;

        // este primita informatia ce contine cine este procesul coordonator al worker-ului curent
        MPI_Recv(&cluster_coordinator, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        // este primita informatia ce contine numarul de elemente din vectorul cu informatii despre topologie
        MPI_Recv(&clusters_info_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        clusters_info = calloc(clusters_info_size, sizeof(int));

        // este primit vectorul cu informatii despre topologie
        MPI_Recv(&clusters_info[0], clusters_info_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);


        // este prelucrat vectorul cu informatii, elementele fiind organizate in functie de clusterele de care apartin
        // prima parte din vector face referire la cluster-ul 0
        // a doua parte din vector face referire la cluster-ul 1
        // a treia parte vector face referire la cluster-ul 2

        int m = 0, n = 0;

        cluster_0_num_workers = clusters_info[m];
        cluster_0_workers = calloc(cluster_0_num_workers, sizeof(int));

        m++;

        while(n < cluster_0_num_workers) {
            cluster_0_workers[n] = clusters_info[m];
            m++;
            n++;
        }

        cluster_1_num_workers = clusters_info[m];
        cluster_1_workers = calloc(cluster_1_num_workers, sizeof(int));

        m++;
        n = 0;

        while(n < cluster_1_num_workers) {
            cluster_1_workers[n] = clusters_info[m];
            m++;
            n++;
        }

        cluster_2_num_workers = clusters_info[m];
        cluster_2_workers = calloc(cluster_2_num_workers, sizeof(int));

        m++;
        n = 0;

        while(n < cluster_2_num_workers) {
            cluster_2_workers[n] = clusters_info[m];
            m++;
            n++;
        }


        // este afisata topologia obtinuta
        display_topology(rank, cluster_0_workers, cluster_0_num_workers, cluster_1_workers, cluster_1_num_workers, cluster_2_workers, cluster_2_num_workers);

        // este receptionat numarul de task-uri ce urmeaza sa fie primite
        MPI_Recv(&worker_num_tasks, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        tasks = calloc(worker_num_tasks, sizeof(int));

        // sunt primite task-urile
        MPI_Recv(&tasks[0], worker_num_tasks, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        // se efectueaza operatia dorita (inmultirea cu 2)
        for(int i = 0; i < worker_num_tasks; i++) {
            tasks[i] = 2 * tasks[i];
        }

        // sunt trimise rezultatele obtinute la procesul coordonator
        printf("M(%d,%d)\n", rank, cluster_coordinator);
        MPI_Send(&tasks[0], worker_num_tasks, MPI_INT, cluster_coordinator, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

}
