Puiu Sebastian-Andrei
	331CC

					Tema #3
			-Calcule colaborative in sisteme distribuite-


	In cadrul acestei teme, am impartit programul in 4 caterogrii, folosind 
instructiuni conditionale. Exista cazul in care procesul curent are rank-ul 
egal cu 0, rank-ul egal cu 1, rank-ul egal cu 2 sau rank-ul mai mare decat 2. 
In cazul in care procesul are rank-ul egal cu 0, acesta se va ocupa mai intai 
de citirea din fisierul care ii este destinat. Dupa care, el le va trimite 
informatia ce contine numarul de workeri din cluster, care ii subordonati lui, 
proceselor 1 si 2. In continuare, el va primi acelasi tip de informatie de la 
procesele 1 si 2. Primirea numarului de workeri de la fiecare coordonator este 
necesara pentru a stii cate id-uri de workeri urmeaza sa primeasca de la acestia. 
Prin urmare, procesul 0 va trimite id-urile workerilor sai catre procesele 1 si 2. 
In continuare, el va primi id-urile workerilor ce apartin proceselor 1 si 2. 
Pentru categoriile in care procesul are rank-ul 1 sau 2, am procedat similar ca 
in cele explicate anterior. Dupa ce toate aceste procese coordonator au obtinut 
unul de la altul toate informatiile necesare pentru stabilirea topologiei, o 
pot afisa. In continuare, fiecare din procesele 0, 1 si 2 vor trimite workerilor 
subordonati fiecaruia atat rank-ul lui (pentru ca workerul sa poata afla cine este 
coordonatorul lui), cat si topologia pe care tocmai au alcatuit-o. Prin urmare, 
toate procesele cu rank mai mare decat 2 se vor ocupa, mai intai, de receptionarea 
acestor date. Pentru a putea realiza transmiterea topologiei, fiecare proces 
coordonator si-a construit un vector ce contine informatiile despre aceasta, in 
felul urmator: pe pozitia 0 se afla n0, unde n0 reprezinta numarul de workeri din 
clusterul 0. Pe urmatoarele n0 pozitii de la acel punct se afla workerii ce compun 
clusterul 0. Pe pozitia 1+n0 se afla n1, unde n1 reprezinta numarul de workeri din 
clusterul 1. Pe urmatoarele n1 pozitii de la acel punct se afla workerii ce compun 
clusterul 1. Pe pozitia 1+n0+1+n1 se afla n2, unde n2 reprezinta numarul de workeri din 
clusterul 2. Pe urmatoarele n2 pozitii de la acel punct se afla workerii ce compun 
clusterul 2. Astfel, toate procesele cu rankul mai mare decat 2 vor primi topologia de 
la coordonatorul fiecaruia. In urma descarcarii si organizarii pe clustere a elementelor 
din vectorului de informatii, procesele de tipul worker vor afisa topologia. Dupa 
finalizarea acestei etape, procesul 0 va prelua argumentele primite de program. Pentru 
a fi mai simplu, am considerat ca primul argument reprezinta numarul de taskuri primite. 
In continuare, am construit vectorul de taskuri, ce contine pe fiecare pozitie 
valoarea pozitiei. Apoi, acesta incearca sa faca o impartire cat mai echilibrata 
a taskurilor, in functie de numarul de workeri din fiecare cluster. Acesta va 
trimite atat numarul de taskuri, cat si taskurile propriu-zise catre procesele 1 
si 2. Procesele 1 si 2 vor prelua mai intai numarul de taskuri, pentru a stii 
care taskuri vor urma sa receptioneze, iar apoi vor prelua taskurile propriu-zise. 
In continuare, procesele coordonator vor imparti numarul de taskuri la numarul de 
workeri pentru a exista o impartire cat mai echilibrata a lor. Exista posibilitatea 
ca impartirea sa nu fie exacta, iar din acest motiv se va tine cont si de restul 
impartirii, care reprezinta numarul taskurilor ce au ramas nedistribuite. Din acest 
motiv, procesul coordonator ii va trimite fiecarui worker numarul de taskuri calculat 
initial, la care se mai adauga inca unul, cat timp inca nu s-au epuizat takurile ce la 
inceput erau nedistribuite. Procesele cu rankul mai mare decat 2 vor primi, prin urmare, 
atat numarul de taskuri alocat lor (pentru a afla cate taskuri urmeaza sa receptioneze), 
cat si taskurile propriu-zise. In continuare, fiecare worker se va ocupa de taskurile 
sale, ce presupun, de fapt, inmultirea cu 2 e elemntelor din vectorul de taskuri. Dupa 
acest pas, ei vor trimite catre coordonatorul fiecaruia rezultatele obtinute. Asadar, 
procesele 0, 1 si 2 se vor ocupa acum de primirea acestor rezultate. Apoi, procesele 
1 si 2 ii vor trimite proceseului 0 rezultatele obtinute in clusterul fiecaruia, iar 
procesul 0 se va ocupa de receptionarea lor. In final, procesul 0 va sorta in mod 
crescator toate rezultatele primite si le va afisa.

