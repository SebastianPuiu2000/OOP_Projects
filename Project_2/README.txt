Puiu Sebastian-Andrei
	331CC

					Tema #2 
		-Procesarea de documente folosind paradigma Map-Reduce-


	In cadrul acestei teme, mai intai am parsat argumentele primite la executie, 
dupa care am efectuat citirea datelor din fisierul de input primit. In continuare, 
am construit task-urile de tip Map. Task-ul de tip Map este alcatuit din numele 
documentului, secventa/fragmentul din document pe care il contine, dimensiunea 
lui, offset-ul la care se afla acesta si, nu in ultimul rand, id-ul task-ului. 
Pentru a putea construi aceste task-uri, am deschis (pe rand) toate documentele 
gasite in fisierul de intrare si le-am prelucrat. Intrucat am citit caracter cu 
caracter, prelucrarea unui fisier se executa cat timp mai exista caractere de citit 
in documentul respectiv. Caracterele citite sunt concatenate la o secventa (numita 
'sequence' in program), iar cand este atinsa dimensiunea dorita a fragmentului, se 
va construi un task de tip Map. La fiecare nou task se tine cont de cate caractere 
au fost citite inainte, pentru a determina offset-ul si, de asemenea, 'sequence' 
este reinitializat. Insa, exista cazul in care s-a ajuns la sfarsitul documentului, 
iar dimensiunea dorita a ultimului fragment nu a fost si nu va mai fi putea atinsa. 
Acest caz a fost tratat prin construirea unui ultim task, unde se verifica doar 
faptul ca nu mai exista caractere de citit si nu se mai tine cont de dimenisunea 
fragmentului dorit. In continuare, pentru a realiza toate task-urile de tip Map, 
m-am folosit de ExecutorService, cu un thread pool al carui numar de threads este 
egal cu numarul de workeri primiti la inceput ca si argument. In acest pool sunt 
adaugate task-urile de Map, fiecare din acestea urmand sa fie preluat de catre un 
worker disponibil. Toti workerii vor executa anumite operatii in cadrul unui task. 
In primul rand, se deschide documentul din task pentru a putea incepe citirea. 
Dupa care, se verifica daca offset-ul la care se afla fragmentul este un numar nul 
sau pozitiv. In cazul in care este egal cu 0, inseamna ca fragmentul se afla la 
inceputul fisierului, deci nu mai este nevoie sa verific daca mai exista caractere 
inainte. Daca offset-ul este diferit de 0, inseamna ca exista sansa ca fix inainte 
de acest fragment sa avem o litera, deci, prin urmare, inceputul lui sa se afle in 
mijlocul altui cuvant. Pentru a verifica aceasta posibilitate, am sarit peste toate 
caracterele de dinaintea celui aflat la acel offset, cu exceptia predecesorului 
sau. Daca acesta este reprezentat de un separator, inseamna ca fragmentul nu incepe 
in mijlocul altui cuvant. Daca acesta este reprezentat de un caracter alphanumeric, 
este evident ca fragmentul in cauza incepe in mijlocul altui cuvant. Daca ne aflam 
pe cazul in care fragmentul incepe in mijlocul unui cuvant, doar trebuie ignorate 
toate caractele alphanumerice pana la intalnirea unui separator. Astfel, acest caz 
va fi redus la cel in care fragmentul nu incepe in mijlocul unui cuvant. In 
continuare, se citeste cat timp inca nu au fost citite mai multe caractere decat 
permitea parametrul de dimensiune din componenta task-ului. In momentul in care se
atinge aceasta dimensiune, apar doua noi cazuri. Primul este cel in care citirea 
se incheie intr-un caracter de tip separator, caz in care nu este nevoie sa mai 
facem nimic. Al doilea este cel in care citirea se incheie intr-un caracter de tip 
alphanumeric, caz in care citirea va trebui sa mai continuie, cat timp nu s-a gasit 
un caracter de tip separator. Mentionez ca in program toate citirile pot fi efectuate 
atata timp cat mai exista caractere necitite in document. In final, se obtine o 
secventa din mai multe caractere alipite, caractere ce au fost obtinute in urma 
citirii (detaliata in randurile anterioare). Aceasta secventa va fi "sparta" in mai 
multe cuvinte, folosind o lista de separatori (data in enuntul din pdf), iar cu 
ajutorul acestor cuvinte se va putea forma o lista cu cele mai lungi cuvinte si un 
dictionar. La fiecare nou cuvant, se verifica daca exista in dictionar lungimea sa. 
Daca nu exista, aceasta va fi adaugata ca si cheie, iar valoarea asociata ei va 
fi 1 (exista doar cuvantul curent cu aceasta lungime). In caz contrar, va fi 
incrementata valoarea asociata cheii cu aceasta. Lista de cuvinte ce au lungime 
maxima va fi construita in modul descris in randurile ce urmeaza. Daca lungimea 
maxima gasita pana acel moment este mai mica decat lungimea cuvantului curent, 
atunci lungimea maxima va fi actualizata, iar lista cu cele mai lungi cuvinte va 
fi reinitializata. Daca lungimea cuvantului curent este egala cu lungima maxima 
gasita pana atunci, atunci acest cuvant va putea fi adaugat in lista celor mai 
lungi cuvinte. Aceste operatii vor fi executate pana cand se termina toate 
cuvintele. Atat aceste rezultate, cat si numele documentului din care au fost 
extrase datele necesare pentru prelucrari, vor fi adaugate intr-un vector de 
rezultate, pe o pozitie ce corespunde cu id-ul task-ului. Aceasta atribuire 
marcheaza finalul executiei unui task de tip Map. In continuare, se doreste 
construirea task-urilor de tip Reduce, pornind de la rezultatele obtinute la 
etapa de Map. Task-ul de tip Reduce este alcatuit din numele documentului, lista 
cu elementele de tip dictionar, lista cu listele ce contin cele mai lungi cuvinte 
gasite in fragmente si, nu in ultimul rand, id-ul task-ului. In cadrul acestei 
cerinte, am realizat atatea task-uri cate documente exista. Astfel, fiecare task 
va contine un nume diferit de document. Prin urmare, principiul de functionare 
este urmatorul: toate rezultate obtinute de la etapa de Map sunt grupate in functie 
de numele documentului de care apartin, fiind adaugate ori in lista de dictionare 
(daca este un element de tip dictionar), ori in lista cuvintelor de lungime 
maxima (daca este un cuvant). Pentru a executa toate task-urile de tip Reduce, 
m-am folosit de ExecutorService, in mod similar ca la partea de Map. Atunci cand 
un worker porneste un task de tip Reduce, el incepe cu iterarea prin fiecare 
dictionar ce se gaseste in lista. Apoi, pentru fiecare dictionar, se parcurg 
toate cheile si valorile asociate lor (unde cheia reprezinta o lungime de cuvant 
si valoarea asociata ei reprezinta numarul de aparitii ale cuvintelor cu aceasta 
lungime). Daca lungimea maxima este mai mica decat cheia curenta, atunci lungimea 
maxima si numarul ei de aparatii vor fi actualizate, fiindu-le atribuite cheia, 
respectiv valoarea asociata acesteia. In cazul in care cheia este egala cu lungimea 
maxima, atunci la numarul de aparitii va fi adaugata valoarea asociata acestei 
chei. De asemenea, la fiecare pas al iteratie, se recalculeaza suma si numarul 
total de aparitii ale cuvintelor, acestea doua fiind necesare pentru determinarea 
rang-ului. In calculul sumei a fost nevoie de folosirea sirului Fibonacii. In 
continuare, este calculat rang-ul corespunzator documentului, folosind formula 
primita in enuntul temei. Rang-ul, lungimea maxima si numarul de aparitii, grupate 
si adaugate impreuna intr-un vector de rezultate, pe o pozitie ce corespunde cu 
id-ul task-ului, marcheaza finalul executiei unui task de tip Reduce. In continuare, 
rezultatele obtinute in urma etapei de Reduce sunt sortate de la cel mai mare rang 
la cel mai mic. De asemenea, pana acum au existat doar caile complete catre un 
document, iar deoarece acestea nu mai sunt necesare, am extras din ele doar numele 
documentelor. Sfarsitul executiei acestui program este marcat de scrierea in 
fisierul de output, pentru fiecare document, a urmatoarelor informatii, obtinute 
in urma tuturor operatiilor realizate: numele documentului, rang-ul documentului, 
lungimea maxima a cuvintelor si numarul de aparitii al cuvintelor cu aceasta lungime.