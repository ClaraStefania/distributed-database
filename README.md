Diaconescu Stefania Clara 313CA 2023-2024

## Distributed Database - Tema 2

### Descriere:

   Problema are ca scop dezvoltarea unei baze de date in care se pastreaza
documentele, pentru optimizare folosindu-se un sistem de caching bazat pe
algoritmul LRU.

   Functiile de baza pentru prelucrarea listelor utilizate in program sunt:
* dll_create() - aloca o lista si initializeaza campurile acesteia
* dll_add_nth_node() - adauga un nod in lista pe pozitia n. Se aloca memorie
si sunt create legaturile in functie de locul in care este adaugat nodul.
* dll_remove_nth_node() - intoarce un pointer la nodul de pe pozitia n,
legaturile fiind desfacute
* dll_free() - dezaloca memoria ocupata de lista

   Functiile de baza pentru prelucrarea hashtable-urilor utilizate in program
sunt:
* key_val_free_function() - elibereaza data dintr-o structura de tip info_t
* ht_create() - aloca si initializeaza un hashtable
* ht_has_key() - verifica daca pentru cheia key a fost asociata anterior o
valoare
* ht_get() - returneaza valoarea asociata cheii
* ht_put() - adauga un element in hashtable
* ht_remove_entry() - elimina din hashtable intrarea asociata cheii key
* ht_free() - elibereaza memoria

   Functiile de baza pentru prelucrarea cozii utilizate in program sunt:
* q_create() - aloca si initializeaza o coada
* q_get_size() - intoarce numarul de elemente din coada
* q_is_empty() - verifica daca este goala coada
* q_front() - intoarce primul element din coada, fara sa il elimine
* q_dequeue() - elimina ultimul element din coada
* q_clear() - elimina toate elementele din coada primita ca parametru
* q_free() - elibereaza memoria

    In 'main()' sunt citite si prelucrate comenzile, urmand sa fie apelate
functiile 'apply_requests()' si 'loader_forward_request()', ce returneaza si
printeaza raspunsul. In rezolvarea temei am realizat functiile pentru un
singur server.

### init_server()

   Se aloca si initializeaza o structura de tip 'server' prin apelarea
functiilor 'init_lru_cache()' care aloca si initializeaza o structura de tip
lru_cache, 'dll_create()' si 'q_create()'.

### server_handle_request()

   Se verifica tipul comenzii. Daca aceasta este EDIT_REQUEST, este bagata in
coada si se afiseaza mesajul corespunzator. Daca este GET_REQUEST, sunt
executate toate comenzile care se afla in coada, pentru fiecare fiind apelata
functia 'server_edit_document()' si printat raspunsul. La final este apelata
functia 'server_get_document()', iar raspunsul este returnat.

### server_edit_document()

   Se verifica prima data daca documentul se afla in cache, apoi este cautat
in database.
- Daca nu este gasit, este este adaugat in cache si in database. Mesajul
afisat este ales in functie de variabila evicted_key care retine documentul
eliminat din cache, daca este cazul.
- Daca este gasit in database, dar nu si in cache, este adus din memoria
principala in cache continutul asociat documentului existent si ii este
modificata valoarea. Mesajul afisat este ales in functie de variabila
evicted_key care retine documentul eliminat din cache, daca este cazul.
- Daca este gasit in cache, se verifica daca documentul se afla deja in lista
de ordine si este adaugat la inceputul listei. La final este modificat
continutul si afisat mesajul.

### server_get_document()

   Se verifica prima data daca documentul se afla in cache, apoi este cautat
in database.
- Daca nu este gasit, este afisat un mesaj de eroare.
- Daca este gasit in database, dar nu si in cache, este adus din memoria
principala in cache continutul asociat documentului existent si ii este
returnata valoarea. Mesajul afisat este ales in functie de variabila
evicted_key care retine documentul eliminat din cache, daca este cazul.
- Daca este gasit in cache, se verifica daca documentul se afla deja in lista
de ordine si este adaugat la inceputul listei. La final este returnat
continutul si afisat mesajul.

### free_server()

   Este eliberata memoria ocupata de o structura de tip server, prin apelarea
functiilor 'free_lru_cache()' care elibereaza o structura de tip lru_cache,
'q_free()' si 'dll_free()'.

   Pentru a adauga si a elimina din cache am utilizat functiie:
* lru_cache_put() - functia verifica daca mai este loc in cache. In cazul in
care este plin, se extrage cel mai vechi document din cache si este adaugat
cel nou, atat in hashtable, cat si in lista de ordine. Daca documentul se
afla deja in hashtable, este returnat false. In caz contrar, se returneaza
true.
* lru_cache_remove() - functia elimina din cache intrarea asociata cheii date
ca parametru si elimina toate aparitiile documentului in lista de ordine.
