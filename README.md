313 Ciocea Bogdan-Andrei

		Pentru abordarea acestei teme, am considerat ca ar fi decent sa fac o 
	structura care sa retina datele unei imagini, adica tipul (avand doua 
	variabile de tip char, prima care sa retina litera “P” si a doua sa retina 
	cifra corespunzatoare tipului imaginii), dimensiunile, valoarea maxima pe 
	care o poate avea un pixel si matricea de date unde se gasesc valorile 
	pixelilor. In loc sa fac o singura matrice pentru toate tipurile de 
	imagine, am ales sa le fac structurat: o singura matrice pentru imaginea
	de tip PGM si trei matrici pentru imaginea de tip PPM (o matrice are 
	valorile culorii rosu, alta pentru verde si a treia pentru albastru).

		O prima functie in acest program este functia “init()”, care 
	initializeaza valorile datelor unei imagini care nu a fost incarcata 
	inca. Aceasta functie a fost facuta ca sa nu avem valori neinitializate.

		A doua functie din program este “eliberare_matrice()”, care elibereaza 
	datele dintr-o imagine, adica dealoca spatiul stocat in matricile alocate, 
	adica daca imaginea este de tip grayscale si, normal, i s-a alocat memorie,
	atunci se va elibera memorie din acea matrice. Daca imaginea ese de tip 
	PPM, atunci se vor elibera cele trei matrici alocate.

		A treia functie este “skip_comments()”, care ne ajuta sa sarim peste 
	comentarii din imagine in cazul in care exista.

		Functia “find_max()” este o functie care gaseste elementul cu valoarea
	maxima dintr-o imagine sau dintr-un set de trei matrici. Daca imaginea 
	este de tip PGM, atunci se va gasi maximul din acea matrice. Daca matricea 
	este de tip PPM, atunci o sa cautam trei valori maxime, fiecare avand o 
	matrice la care se refera, apoi comparam maximele. Aceasta functie este 
	facuta sa actualizeze valoarea maxima dintr-un pixel dupa o operatie 
	(de exemplu cand vrem sa taiem o imagine dupa o selectie folosind functia 
	“CROP()”, trebuie sa actualizam valoarea numarului maxim, ca altfel nu se 
	va salva corect o imagine).

		Pentru a modulariza codul si a nu va “copy-paste” de cateva ori, am 
	decis sa fac o functie care aloca memorie unei matrici. Aceasta se numeste 
	“alocare_matrice()”, care are drept parametrii numarul de linii si numarul 
	de coloane. Aceasta functie returneaza o matrice caruia i s-a alocat 
	dinamic memorie. De asemenea, in aceasta functie exista si programarea 
	defensive, astfel, daca se depaseste memoria sa se dealoce memoria si sa 
	nu intoarca nimic.

		Functia “alocareppm()” practic foloseste functia “alocare_matrice()” 
	pentru cele trei matrici care contin valorile culorilor.

		Functia “alocarepgm()” aloca memoria unei matrici, adica cea a unei 
	imagini de tip PGM.

		Functia “citire_binar()” citeste datele dintr-o matrice, fie de tip 
	PGM, fie PPM. Adica aceasta functie deschide imaginea cu scopul de a 
	citi datele in format binar. Se citesc datele referitor la tipul imaginii 
	(dar in mod fictiv, deoarece tipul de imagine se citeste in functia 
	“LOAD()”), la numarul de coloane si de linii, si apoi se verifica tipul 
	de imagine. Verificam daca tipul de imagine este PPM sau PGM si se aloca 
	memorie pentru matricele corespunzatoare. Se citesc matricele din imagine 
	cu functia “fread()”. Totodata, la aceasta functie se sar si peste 
	comentarii daca exista.

		Functia “citire_ascii()” face exact acelasi lucru ca si functia 
	explicate mai sus, dar se citesc datele sub forma de intregi, nu caractere.
	Avem functia “LOAD()”, in care sunt introduse aceste doua functii. Inainte 
	de toate avem variabla pointer “*OK”, care verifica daca a fost inainte 
	introdusa o imagine. Daca da, atunci trebuie sa dealocam matricele alocate 
	si sa initializam structura din nou si valoarea pointerului la OK se va 
	face 0. Apoi o sa deschidem fisierul pe care vrem sa il incarcam in 
	structura doar pentru a-l citi. Citim tipul imaginii si sarim peste 
	posibilele comentarii. Apoi o sa inchidem fisierul si o sa citim fisierul 
	dupa tipul de imagine inregistrat in structura. Dupa aceea OK o sa aiba 
	valoarea 1 ce indica faptul ca fisierul a fost incarcat cu succes.

		Functia “SELECT_ALL()” este cu scopul de a seta coordonatele de 
	selectie in coltul din stanga sus si in coltul din dreapta jos. 
	Inainte de a se face aceasta operatie, se verifica daca exista un fisier 
	incarcat.

		Functia “SELECT()” practic seteaza doua coordonate de selectie. Acestea
	se vor citi in main(), si se vor verifica daca sunt valide in aceasta 
	functie. Intai verificam daca exista un fisier deja incarcat, pentru ca 
	operatia nu ar mai avea loc daca nu s-a incarcat nicio imagine. Apoi o sa 
	verificam daca coordonatele sunt valide, adica daca acestea macar nu ies 
	din matrice si nici daca sunt egale fiecare. Dupa treaba aceasta, se 
	verifica daca avem ordinea coordonatelor incorecta, adica daca, din 
	greseala, am introdus intai coordonata din dreapta jos si apoi coordonata 
	din stanga sus, ca apoi, daca este adevarat acest caz, sa rectificam 
	aceasta greseala si sa inversam coordonatele ele intre ele. Dupa aceea, 
	variabilele care reprezinta coordonatele din structura va lua valorile 
	acelor coordonate.

		Functia “HISTOGRAM()” face histograma unei imagini de tip PGM. Intai de
	toate se verifica daca exista o imagine incarcata, ca de nu, o sa se arate 
	mesajul “No image loaded”. Apoi se vor citi coordonatele si se va verifica 
	daca nu este trecuta una dintre ele sau ambele. Apoi se verifica daca cumva
	tipul imaginii este color. Dupa se va verifica daca numarul de binuri este 
	valid. Dupa aceea se vor face efectiv operatiile descrise in fisierul temei
	de casa. Adica facem un vector care sa retina frecventa fiecarui pixel in 
	imagine, ca dupa sa facem maximul de frecventa in binuri si dupa aceea sa 
	calculam numarul de stelute si sa le afisam cum ni s-a cerut.

		Functia “EXIT()”  dealoca resursele matricelor alocate in cazul in care
	exista o imagine incarcata. Daca nu, atunci se va afisa mesajul “No image 
	loaded”. Tin sa precizez ca dupa aplicarea acestei functii se va iesi din 
	program, indiferent de faptul ca imaginea a fost incarcata sau nu.

		Functia “CROP_PGM()”  se taie imaginea de tip PGM dupa o selectie 
	anume. Aceasta functie practic face o matrice auxiliara cu datele matricei 
	taiate. Noi vom actualiza numarul de linii si numarul de coloane. Apoi o sa
	eliberam matricea alocata imaginii din structura. Actualizam dimensiunile 
	imaginii din structura si apoi alocam memorie pentru matricea din imagine 
	cu dimensiunile noi ale imaginii. Pe urma se copiaza efectiv valorile 
	matricei auxiliare in matricea din structura. Dupa se va dealoca memoria 
	pentru matricea auxiliara, iar selectia va fi setata drept toata imaginea.

		Functia “free_matrici_color()” dealoca memoria pentru trei mattrici cu 
	“height” linii.

		Functia “CROP_PPM()” face acelasi lucru ca si functia “CROP_PGM()”, 
	singura diferenta ar fii ca in loc de o matrice auxiliara sunt trei, 
	pentru fiecare culoare (R, G, B).

		Functia “CROP()” imbina cele doua functii de crop de mai sus, 
	verificand tipul imaginii, dar inainte de aceasta intructiune, verifica 
	daca imaginea este incarcata. Daca operatia s-a finalizat cu success, se 
	va afisa mesajul “Image cropped”.

		Functiile “SAVE_ASCII()” si “SAVE_BINARY()” sunt practic in oglinda cu 
	functiile “citire_ascii()” si “citire_binar()”, adica pe cand functiile de 
	citire citesc datele, functiile de salvare afiseaza in alt fisier imaginea.
	Adica, daca la functia de citire in ascii avem functia “fscanf()”, la 
	salvarea elementelor sub formatul ASCII avem “fprintf()” si la citirea 
	elementelor in format binar avem “fread()”, la salvarea elementelor in 
	format binar avem functia “fwrite()”, care scrie intr-un fisier matricea 
	sub format binar. Aceste doua functii verifica inainte de toate daca 
	imaginea a fost inainte incarcata. Daca acest lucru se respecta, atunci se 
	va deschide fisierul sub format binar sau ascii (depinde de caz). In ambele
	functii se verifica inainte de scrierea elementelor din matrice tipul 
	matricei, adica daca noi vrem sa transformam o matrice din format binar in 
	format ascii, ii schimbam tipul si vice versa.

		Functia “EQUALIZE()” face ce se cere din enuntul problemei, adica sa 
	egalizeze pixelii. Acest algoritm merge in felul urmator: verifica daca 
	exista o imagine incarcata, daca intr-adevar este incarcata o matrice, 
	atunci ii vom verifica tipul, deoarece noi dorim sa egalizam o imagine de 
	tip PGM (grayscale) si vom afisa mesajul “Black and white image needed” 
	daca imaginea este color. Se face un vector de frecventa care sa ia 
	frecventa fiecarui pixel din imagine. Apoi, fiecare pixel va lua valoarea 
	data de formula scrisa in enuntul temei: se face o variabila care sa retina
	valoarea ariei imaginii, avem o suma de frecvente si avem calculul 
	propriu-zis. Se rotunjeste rezultatul si se verifica daca valoarea apartine
	intervalului [0, 255]. Dupa aceea, valoarea pixelului va lua valoarea 
	variabilei “rez” si se va afisa mesajul “Equalize done”.

		Functiile “inmultire_matrici()” si “inmultire_matrici_n()” sunt legate 
	intre ele, deoarece ambele au fost create ca sa faca operatia (babeste) a 
	inmultirilor fiecarui pixel cu o valoare dintr-o matrice data. Singura 
	chestie care difera una fata de cealalta este ca functia 
	“inmultire_matrici_n()” merge doar pe functiile de “BLUR” si 
	“GAUSSIAN_BLUR” din comanda “APPLY”.  Functia “inmultire_matrici_n()” 
	imparte rezultatul la un n dat (depinde de functie). Aceste doua functii 
	folosesc o matrice in care fiecare element reprezinta un produs de doua 
	elemente din doua matrici. Exista o matrice auxiliara cu elementele 
	matricei corespunzatoare pentru a nu deranja ceilalti pixeli cu valoarea 
	actualizata al pixelului modificat. De asemenea mai avem si matricea “mat” 
	care reprezinta matricea specifica fiecarei operatii din comanda “APPLY”, 
	adica poate reprezenta fiecare matrice din enuntul temei.

		Functia “apply_inmultire()” practice face operatiile de “APPLY” pentru 
	“SHARPEN” si “EDGE”. Aceasta functie merge doar pe o selectie anume, fie ca
	e una mica, fie ca este pe toata imaginea. Avem pentru fiecare culoare cate
	o matrice de dimensiuni 3 x 3 in care se vor face inmultirile si apoi se 
	vor aduna valorile lor. Se verifica daca selectia cumva nu este pe ramura, 
	adica pe margini, deoarece nu are ce calcule sa se faca cu pixelii din 
	marginile imaginii, deoarece acei pixeli nu au 8 vecini, deci se vor 
	ajusta. Dupa aceea se aloca dinamic in memorie o matrice de “image->height”
	linii si “image->width” coloane si se copiaza elementele dintr-o matrice de
	culoare in una din matricile auxiliare (adica pentru fiecare culoare exista
	o matrice auxilara si se face operatiile pe fiecare pixel separate) ca, pe 
	urma sa se dealoce memoria pentru matricele noi alocate ca sa nu avem 
	memory leak-uri (scuzati rom-engleza…nu ma depunctati va rog). Aceasta 
	functie este folosita doar in operatiile de “EDGE” si de “SHARPEN”.
	Pentru functia “apply_inmultire_blur()” se face acelasi lucru ca in functia
	de mai sus, doar ca se cere sa se imparta rezultatul sumei produselor la un
	anumit numar, deci aceasta functie va fi folosita in functiile de “BLUR” si
	de “GAUSSIAN_BLUR”.

		Urmatoarele patru functii de mai jos au acelasi stil, doar ca difera 
	matricele. Acestea sunt functiile in care se aplica un efect pe o imagine. 
	Acestea au structura asa: se verifica daca imaginea a fost incarcata in 
	memorie, daca nu atunci se va afisa mesajul “No image loaded”. Daca 
	imaginea este incarcata si este de tip PGM, atunci se va afisa mesajul 
	“Easy, Charlie Chaplin”. Daca imaginea exista si este de tip color, atunci 
	vom avea o matrice de dimensiune 3 x 3 si apoi vom apela functia pentru 
	inmultirea elementelor din matrici. La final se va afisa un mesaj de 
	incheiere cu success al functiei.
	
		In functia “main()” se apeleaza toate functiile, se declara structura 
	imagine, se initializeaza structura cu functia “init()”, se declara comanda
	“line”, “*tok” pe care il vom folosi atunci cand o sa ne dorim sa spargem 
	un sir in cuvinte si o variabila “OK”, care ne arata daca o imagine a fost 
	incarcata in memorie sau nu. Apoi, cu ajutorul unui “while”, vom apela 
	functiile. Daca ce scriem nu se regaseste printre cele din functia 
	“main()”, atunci se va afisa mesajul “Invalid command”.
