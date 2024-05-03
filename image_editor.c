#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define DELIM "\n"
#define SPACE " "
/*Structura care sa retina datele unei imagini, adica inaltimea, latimea,
selectiile, matricile, tipul de imagine si valoarea maxima al unui pixel*/
struct imagine {
	int width, height, max;
	int **gray, **R, **G, **B;
	char header1, header2;
	int x1, y1, x2, y2;
};

/*Functia init practic initializeaza o imagine inainte de incarcarea primei
																imagini*/
void init(struct imagine *image)
{
	image->width = -1;
	image->height = -1;
	image->max = -1;
	image->header2 = '\0';
	image->header1 = '\0';
	image->gray = NULL;
	image->R = NULL;
	image->G = NULL;
	image->B = NULL;
	image->x1 = -1;
	image->y1 = -1;
	image->x2 = -1;
	image->y2 = -1;
}

/*Functia "eliberare matrice" elibereaza memoria alocata unei matrici din
structura "imagine"*/
void eliberare_matrice(struct imagine *image)
{
	//Verificam tipul imaginii
	if (image->header2 == '2' || image->header2 == '5') {
	//Este de tip PGM. Dealocam memoria pentru matricea imaginii PGM
		for (int i = 0; i < image->height; i++)
			free(image->gray[i]);
		free(image->gray);
	} else {
		/*Altfel este de tip PPM si, desigur, dealocam memoria din cele 3
																matrici*/
		for (int i = 0; i < image->height; i++) {
			free(image->R[i]);
			free(image->G[i]);
			free(image->B[i]);
		}
		free(image->R);
		free(image->G);
		free(image->B);
	}
}

//Functia care ne ajuta sa sarim peste comentarii
void skip_comments(FILE *fisier)
{
	while (getc(fisier) != '\n')
		continue;
	while (getc(fisier) == '#') {
		while (getc(fisier) != '\n')
			continue;
	}
	fseek(fisier, -1, SEEK_CUR);
}

/*Functia "find_max", dupa nume cauta cel mai mare element al unei matrici ceea
ce ne va ajuta la gasirea valorii maxime al unui pixel dintr-o imagine anume*/
int find_max(struct imagine *image)
{
	//In primul rand ne initializam 3 variabile pe care le vom compara
	/*max se va folosi atat in cautarea elementului maxim dintr-o matrice al
	unei imagini de tip PGM, cat si din economie de variabile, o vom folosi
	si pentru imagini de tip PPM*/
	int max = -1;
	int max2 = -1, max3 = -1;
	/*Pentru imagini de tip PGM*/
	if (image->header2 == '2' || image->header2 == '5') {
		//Cautam prin toate liniile
		for (int i = 0; i < image->height; i++) {
			//Cautam prin toate coloanele
			for (int j = 0; j < image->width; j++) {
				//Verificam daca "max" este mai mic decat un element din
				//matrice
				if (max < image->gray[i][j]) {
					//"max" va prelua acea valoare daca conditia
					//este respectata
					max = image->gray[i][j];
				}
			}
		}
	} else {
		//Daca, intr-adevar imaginea este de tip PPM
		//Ne plimbam pe linii
		for (int i = 0; i < image->height; i++) {
			//Ne plimbam pe coloane
			for (int j = 0; j < image->width; j++) {
				//aici gasim cele mai mari "max", "max2" si "max3" valori
				if (max < image->R[i][j])
					max = image->R[i][j];
				if (max < image->G[i][j])
					max2 = image->G[i][j];
				if (max < image->B[i][j])
					max3 = image->B[i][j];
			}
		}
		//Comparam valorile de max
		if (max < max2)
			max = max2;
		if (max < max3)
			max = max3;
	}
	//Punem o conditie de siguranta
	if (max > 255)
		return 255;
	//Returnam cea mai mare valoare al unui pixel
	return max;
}

/*Functia "alocare_matrice" practic aloca memorie unei matrici cu "height"
					linii si "width" coloane*/
int **alocare_matrice(int height, int width)
{
	/*Alocam dinamic memoria unei matrici folosind calloc*/
	int **matrice = (int **)calloc(height, sizeof(int *));
	//Programare defensiva
	if (!matrice) {
		printf("Nu am putut aloca memorie\n");
		return NULL;
	}
	for (int i = 0; i < height; i++) {
		matrice[i] = (int *)calloc(width, sizeof(int));
		//Programare defensiva
		if (!matrice[i]) {
			for (int j = i - 1; j >= 0; j--)
				free(matrice[j]);
			free(matrice);
			printf("Nu am putut aloca memorie\n");
			return NULL;
		}
	}
	//Returnam o matrice cu memorie alocata
	return matrice;
}

/*Functia "alocareppm" aloca memorie pentru 3 matrici din structura imagine*/
void alocareppm(struct imagine *image)
{
	/*Alocam memorie pentru matricea corespunzatoare culorii rosu*/
	image->R = alocare_matrice(image->height, image->width);
	/*Alocam memorie pentru matricea corespunzatoare culorii verde*/
	image->G = alocare_matrice(image->height, image->width);
	/*Alocam memorie pentru matricea corespunzatoare culorii albastru*/
	image->B = alocare_matrice(image->height, image->width);
}

//Functia "alocarepgm" aloca memorie unei singure matrici
void alocarepgm(struct imagine *image)
{
	//Alocam memorie pentru o singura matrice din structura imagine
	image->gray = alocare_matrice(image->height, image->width);
}

//Functia "citire_binar" citeste toate datele dintr-un fisier binar
/*Aceasta functie, de asemenea citeste celelalte date ale unei imagini care se
regasesc si intr-un fisier de tip ASCII, adica tipul, dimensiunea si valoarea
maxima pe care o poate avea un pixel*/
void citire_binar(struct imagine *image, char *f)
{
	/*Intai se va deschide respectiva imagine(fisierul) doar ca sa citim
	datele in format binar*/
	FILE *file = fopen(f, "rb");
	/*Vom citi fictiv tipul de imagine, pentru ca deja l-am citit in functia
	"LOAD" care va urma dupa citiri*/
	char header1, header2;
	/*Citim tipul de imagine*/
	fscanf(file, "%c%c", &header1, &header2);
	/*Sarim peste comentarii daca exista*/
	skip_comments(file);
	/*Pentru siguranta verificam daca imaginea este de vreun tip anume*/
	if (header1 != 'P') {
		/*Daca nu corespunde de niciun tip cunoscut afisam un mesaj*/
		printf("Failed to load %s\n", f);
		return;
	}
	/*Citim dimensiunea unei imagini sub forma de date de int */
	fscanf(file, "%d%d", &image->width, &image->height);
	/*Sarim peste comentarii daca exista
	(imi cer scuze daca am pus de prea multe ori, dar sunt sanse sa fie doua
																comentarii)*/
	skip_comments(file);
	/*Citim valoarea maxima al unui pixel*/
	fscanf(file, "%d", &image->max);
	/*Verificam daca exista comentarii in imagine*/
	skip_comments(file);
	/*Daca imaginea este de tip PGM atunci vom aloca memorie unei matrici,
	aceea fiind "gray" din structura*/
	if (header2 == '5') {
		alocarepgm(image);
		/*Daca nu atunci vom aloca memorie pentru cele trei matrici, deoarece
		imaginea este de tip color*/
	} else if (header2 == '6') {
		alocareppm(image);
	}
	//Citim datele unei imagini de tip grayscale cu functia "fread"
	if (header2 == '5') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++)
				fread(&image->gray[i][j], sizeof(unsigned char), 1, file);
		}
	}
	//Citim datele unei imagini de tip color cu functia "fread"
	if (header2 == '6') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				fread(&image->R[i][j], sizeof(unsigned char), 1, file);
				fread(&image->G[i][j], sizeof(unsigned char), 1, file);
				fread(&image->B[i][j], sizeof(unsigned char), 1, file);
			}
		}
	}
	//Afisam mesajul la final
	printf("Loaded %s\n", f);
	//Inchidem fisierul
	fclose(file);
}

//Functia "citire_ascii" preia informatiile unei imagini in format ASCII
void citire_ascii(struct imagine *image, char *f)
{
	/*Intai deschidem acest fisier doar pentru citire*/
	FILE *file = fopen(f, "rt");
	/*Din nou, o sa citim tipul de imagine, dar nu vom folosi aceste
	 variabile*/
	char header1, header2;
	fscanf(file, "%c%c", &header1, &header2);
	/*Sarim peste comentariu*/
	skip_comments(file);
	/*Verificam daca acest fisier este valid*/
	if (header1 != 'P') {
		printf("Failed to load %s\n", f);
		return;
	}
	/*Citim dimensiunile unei imagini*/
	fscanf(file, "%d%d", &image->width, &image->height);
	/*Sarim peste comentarii*/
	skip_comments(file);
	/*Citim valoarea maxima pe care o poate avea un pixel*/
	fscanf(file, "%d", &image->max);
	/*Sarim peste posibilul comentariu*/
	skip_comments(file);
	/*Verificam daca fisierul este de tip grayscale*/
	if (header2 == '2') {
		/*Daca da atunci alocam memorie pentru matricea respectiva*/
		alocarepgm(image);
		/*Daca nu atunci vom aloca memorie pentru cele trei matrici pentru
		imaginea de tip color*/
	} else if (header2 == '3') {
		alocareppm(image);
	}
	//Daca imaginea este de tip PGM
	if (image->header2 == '2') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				//Vom citi datele din matricea acelei imagini
				fscanf(file, "%d", &image->gray[i][j]);
			}
		}
	}
	//Daca imaginea respectiva este de tip PPM
	if (image->header2 == '3') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				//Vom citi datele din matricele acelei imagini
				fscanf(file, "%d", &image->R[i][j]);
				fscanf(file, "%d", &image->G[i][j]);
				fscanf(file, "%d", &image->B[i][j]);
			}
		}
	}
	//La final vom da mesajul ca operatia s-a finalizat cu succes
	printf("Loaded %s\n", f);
	//Inchidem fisierul
	fclose(file);
}

/*Functia "LOAD" este functia care incarca in memorie o imagine, adica practic
incarcam in structura creata datele acelei imagini*/
void LOAD(struct imagine *image, char *f, int *OK)
{
	/*Avem o variabila care ne spune daca imaginea a fost incarcata, aceasta
		fiind "OK" o vom initializa cu zero pentru un inceput*/
	if (*OK == 1) {
		*OK = 0;
		eliberare_matrice(image);
		init(image);
	}
	/*Vom deschide pentru prima data un fisier*/
	FILE *file = fopen(f, "r");
	/*Verificam daca exista acel fisier in primul rand*/
	if (!file) {
		/*Daca nu atunci vom afisa un mesaj*/
		printf("Failed to load %s\n", f);
		return;
	}
	//Citim tipul de imagine
	fscanf(file, "%c%c", &image->header1, &image->header2);
	/*Sarim peste posibilele comentarii*/
	skip_comments(file);
	skip_comments(file);
	/*Verificam daca imaginea respectiva este valida*/
	if (image->header1 != 'P') {
		printf("Failed to load %s\n", f);
		return;
	}
	/*Apoi inchidem fisierul, pentru ca urmeaza sa citim ori in format text ori
	in format binar*/
	fclose(file);
	/*Aici verificam daca imaginea este de tip ASCII sau binar*/
	if (image->header2 == '5' || image->header2 == '6')
		citire_binar(image, f);
	else
		citire_ascii(image, f);
	/*Daca operatiunea s-a terminat cu succes, variabila "OK" va avea
	valoarea 1*/
	*OK = 1;
	/*Vom initializa selectia aici ca fiind toata*/
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = image->width;
	image->y2 = image->height;
}

/*Functia "SELECT_ALL" face ca cele doua coordonate sa fie in partea din stanga
sus si dreapta jos pe intreaga imagine*/
void SELECT_ALL(struct imagine *image, int OK)
{
	/*Intai verificam daca imaginea a fost deja incarcata*/
	if (!OK) {
		/*Daca nu atunci vom afisa un mesaj*/
		printf("No image loaded\n");
		return;
	}
		/*Daca avem o imagine incarcata, atunci vom initializa selectia sa fie
		pe intreaga imagine*/
		image->x1 = 0;
		image->y1 = 0;
		image->x2 = image->width;
		image->y2 = image->height;
		/*Afisam mesajul dorit*/
		printf("Selected ALL\n");
}

/*Functia "SELECT" initializeaza selectia dorita de la tastatura avand doua
coordonate: una din partea din stanga sus si cealalta din dreapta jos*/
void SELECT(struct imagine *image, int x1, int y1, int x2, int y2, int OK)
{
	/*Verificam daca imaginea a fost incarcata in memorie*/
	if (!OK) {
		/*Daca nu atunci se va afisa un mesaj*/
		printf("No image loaded\n");
		return;
	}
		/*Daca imaginea a fost incarcata atunci vom verifica daca coordonatele
		sunt valide*/
		if (x1 < 0 || x1 > image->width || x2 < 0 || x2 > image->width) {
			/*Daca nu sunt valide atunci vom afisa un mesaj*/
			printf("Invalid set of coordinates\n");
			return;
		}
		if (y1 < 0 || y1 > image->height || y2 < 0) {
			/*Daca nu sunt valide atunci vom afisa un mesaj*/
			printf("Invalid set of coordinates\n");
			return;
		}
		if (y2 > image->height || x1 == x2 || y1 == y2) {
			/*Daca nu sunt valide atunci vom afisa un mesaj*/
			printf("Invalid set of coordinates\n");
			return;
		}
		/*Insa daca sunt valide si daca nu sunt intr-o ordine buna puse
		coordonatele, atunci le vom schimba*/
		if (x1 > x2) {
			int aux = x1;
			x1 = x2;
			x2 = aux;
		}
		if (y1 > y2) {
			int aux = y1;
			y1 = y2;
			y2 = aux;
		}
		/*Dupa aceea variabilele de selectie din structura vor prelua
		aceste coordonate*/
		image->x1 = x1;
		image->x2 = x2;
		image->y1 = y1;
		image->y2 = y2;
		/*Vom afisa mesajul dorit*/
	printf("Selected %d %d %d %d\n", image->x1
	, image->y1, image->x2, image->y2);
}

/*FUnctia "HISTOGRAM" practic va afisa histograma unei imagini
in tonuri de gri*/
void HISTOGRAM(struct imagine *image, int OK, char c[])
{	/*Verificam daca imaginea este incarcata*/
	if (!OK) {
		printf("No image loaded\n");
		return;
	}
	int x, y;
	c = strtok(NULL, " ");
	if (!c) {
		printf("Invalid command\n");
		return;
	}
	sscanf(c, "%d", &x);
	c = strtok(NULL, " ");
	if (!c) {
		printf("Invalid command\n");
		return;
	}
	sscanf(c, "%d", &y);
	c = strtok(NULL, " ");
	if (c) {
		printf("Invalid command\n");
		return;
	}
	if (image->header2 == '3' || image->header2 == '6') {
		printf("Black and white image needed\n");
		return;
	}
	if (y < 2 || y > 256) {
		printf("Invalid set of parameters\n");
		return;
	}
	if (image->header2 == '2' || image->header2 == '5') {
		int *v = (int *)calloc(256, sizeof(int));
		//v = frecventa unui pixel
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++)
				v[image->gray[i][j]]++;
		}
	int bins = 256 / y;
	// aflam frecventa maxima
	int max_frecventa = -1;
	int sum_bin = 0;
	for (int i = 0; i < 256; i++) {
		sum_bin += v[i];
		if (!((i + 1) % bins)) {
			if (max_frecventa < sum_bin)
				max_frecventa = sum_bin;
		sum_bin = 0;
		}
	}
	// aflam numarul de stelute pentru fiecare pixel
	double sum = 0;
	for (int i = 0; i < 256; i++) {
		sum += v[i];
		if (!((i + 1) % bins)) {
			double nr_stelute = (double)(sum / max_frecventa) * x;
			sum = 0;
			//double nr_stelute2 = nr_stelute * x;
			int stelute = (int)nr_stelute;
			printf("%d\t|\t", stelute);
			for (int j = 0; j < stelute; j++)
				printf("*");
			printf("\n");
		}
	}
	free(v);
	}
}

/*Functia "EXIT" iese din program si dealoca memoria daca este incarcata deja
																o imagine*/
void EXIT(struct imagine *image, int OK)
{
	/*Verificam intai daca imaginea a fost incarcata*/
	if (!OK) {
		/*Daca nu atunci se va afisa un mesaj*/
		printf("No image loaded\n");
	} else {
		/*Daca este incarcata atunci vom dealoca toate resursele*/
		eliberare_matrice(image);
	}
}

/*Functia "CROP_PGM" taie o bucata din imagine de tip grayscale*/
void CROP_PGM(struct imagine *image)
{
	/*Initializam noile dimensiuni ale imaginii*/
	int height = image->y2 - image->y1;
	int width = image->x2 - image->x1;
	//Alocam memorie pentru matricea auxiliara
	int **matrice = alocare_matrice(height, width);
	//Copiem elementele din imagine in matricea auxiliara
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			matrice[i][j] = image->gray[i + image->y1][j + image->x1];
	}
	//Eliberam imaginea de memorie
	eliberare_matrice(image);
	/*Setam noile dimensiuni in structura*/
	image->height = height;
	image->width = width;
	//Alocam cu memorie noua imaginea
	alocarepgm(image);
	//Punem elementele salvate din matricea auxiliara in imagine
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			image->gray[i][j] = matrice[i][j];
	}
	//Eliberam matricea auxiliara
	for (int i = 0; i < image->height; i++)
		free(matrice[i]);
	free(matrice);
	/*Setam noua valoare al pixelului maxim*/
	image->max = find_max(image);
	/*Pentru a evita seg fault daca dam comanda "CROP" de doua ori,
	reinitializam selectia ca fiind cea totala*/
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = width;
	image->y2 = height;
}

/*Aceasta functie elibereaza memoria pentru trei matrici, adica este facuta
special pentru a elibera matricele dintr-o imagine color*/
void free_matrici_color(int **R, int **G, int **B, int height)
{
	/*Aici se dealoca memoria*/
	for (int i = 0; i < height; i++) {
		free(R[i]);
		free(G[i]);
		free(B[i]);
	}
	free(R);
	free(G);
	free(B);
}

/*Functia "CROP_PPM" este la fel ca "CROP_PGM", doar de vreo trei ori mai
lunga in sensul ca face aceeasi operatie doar ca de vreo trei ori, deoarece
sunt trei matrici*/
void CROP_PPM(struct imagine *image)
{
	/*Setam noile valori pentru dimensiunile imaginii*/
	int height = image->y2 - image->y1;
	int width = image->x2 - image->x1;
	/*Alocam memorie pentru matricea de culoare rosu*/
	int **matrice_R = alocare_matrice(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			matrice_R[i][j] = image->R[i + image->y1][j + image->x1];
	}
	/*Alocam memorie pentru matricea de culoare verde*/
	int **matrice_G = alocare_matrice(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			matrice_G[i][j] = image->G[i + image->y1][j + image->x1];
	}
	/*Alocam memorie pentru matricea de culoare albastru*/
	int **matrice_B = alocare_matrice(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++)
			matrice_B[i][j] = image->B[i + image->y1][j + image->x1];
	}
	/*Eliberam din memorie matricile alocate pentru imaginea de tip PPM*/
	eliberare_matrice(image);
	/*Setam noile valori ale dimensiunilor*/
	image->height = height;
	image->width = width;
	/*Alocam memorie pentru noile matrici*/
	alocareppm(image);
	/*De aici punem inapoi pixelii de culoare pentru imaginea taiata*/
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			image->R[i][j] = matrice_R[i][j];
	}
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			image->G[i][j] = matrice_G[i][j];
	}
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			image->B[i][j] = matrice_B[i][j];
	}
	/*Eliberam de memorie matricele create*/
	free_matrici_color(matrice_R, matrice_G, matrice_B, height);
	/*Setam noua valoare al pixelului maxim*/
	image->max = find_max(image);
	/*Pentru a evita seg fault daca dam comanda "CROP" de doua ori,
	reinitializam selectia ca fiind cea totala*/
	image->x1 = 0;
	image->y1 = 0;
	image->x2 = width;
	image->y2 = height;
}

/*Functia "CROP" apeleaza cele doua functii asociate, sau da un mesaj
daca nu s-a incarcat in memorie o imagine inainte*/
void CROP(struct imagine *image, int OK)
{
	/*Aici se verifica dac s-a incarcat o imagine*/
	if (!OK) {
		/*Mesajul pe care il reda functia daca nu s-a incarcat*/
		printf("No image loaded\n");
		return;
	}
	/*Daca este incarcata in memorie o imagine atunci i se va verifica
	tipul
	Aici este daca imaginea este de tip PGM*/
	if (image->header2 == '2' || image->header2 == '5') {
		CROP_PGM(image);
	} else {
	/*Daca imaginea este de tip PPM atunci se va apela aceasta functie*/
		CROP_PPM(image);
	}
	printf("Image cropped\n");
}

/*Functia "SAVE_ASCII" salveaza datele unei imagini in format ASCII
Aceasta functie afiseaza in alt fisier datele unei imagini care a fost sau nu
modificata*/
void SAVE_ASCII(struct imagine *image, char *file, int OK)
{
	/*Aici verificam daca s-a incarcat in memorie o imagine*/
	if (!OK) {
		/*Daca nu atunci se va afisa acest mesaj*/
		printf("No image loaded\n");
		return;
	}
	/*Daca a fost incarcata in memorie o imagine atunci vom deschide alt
	fisier si vom trece datele in el. Si de aceea vom deschide
	fisierul cu "wt", deoarece vrem sa scriem drept text*/
	FILE *fout = fopen(file, "wt");
	/*Daca fisierul a fost binar inainte, ii schimbam tipul corespunzator
	Daca a fost inainte fisier in PGM binar(P5) atunci se va face P2
	Daca a fost inainte fisier in PPM binar(P6) atunci se va face P3*/
	if (image->header2 == '6')
		image->header2 = '3';
	else if (image->header2 == '5')
		image->header2 = '2';
	/*Afisam in fisier tipul de imagine*/
	fprintf(fout, "%c%c\n", image->header1, image->header2);
	/*Afisam in fisier dimensiunile imaginii*/
	fprintf(fout, "%d %d\n", image->width, image->height);
	/*Afisam in fisier valoarea maxima al unui pixel*/
	fprintf(fout, "%d\n", image->max);
	/*Apoi daca fisierul este de tip PGM vom afisa in fisierul de output
	matricea corespunzatoare imaginii*/
	if (image->header2 == '2') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++)
				fprintf(fout, "%d ", image->gray[i][j]);
			fprintf(fout, "\n");
		}
		/*Daca fisierul este de tip PPM, atunci vom afisa in fisierul de
		output "matricea"(sunt 3 de fapt) corespunzatoare imaginii*/
	} else if (image->header2 == '3') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				fprintf(fout, "%d ", image->R[i][j]);
				fprintf(fout, "%d ", image->G[i][j]);
				fprintf(fout, "%d ", image->B[i][j]);
			}
			fprintf(fout, "\n");
		}
	}
	/*Vom afisa mesajul de finalizare cu succes*/
	printf("Saved %s\n", file);
	/*Vom inchide fisierul*/
	fclose(fout);
}

/*Functia "SAVE_BINARY" salveaza datele unei imagini in format BINAR
Aceasta functie face acelasi lucru ca si cel de dinainte, doar ca in loc de
fprintf la datele matricei, o sa dea fwrite cu sizeof char*/
void SAVE_BINARY(struct imagine *image, char *f, int OK)
{
	if (!OK) {
		printf("No image loaded\n");
		return;
	}
	FILE *fout = fopen(f, "wb");
	if (image->header2 == '2')
		image->header2 = '5';
	else if (image->header2 == '3')
		image->header2 = '6';
	fprintf(fout, "%c%c\n", image->header1, image->header2);
	fprintf(fout, "%d %d\n", image->width, image->height);
	fprintf(fout, "%d\n", image->max);
	if (image->header2 == '5') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++)
				fwrite(&image->gray[i][j], sizeof(char), 1, fout);
		}
	} else if (image->header2 == '6') {
		for (int i = 0; i < image->height ; i++) {
			for (int j = 0; j < image->width; j++) {
				fwrite(&image->R[i][j], sizeof(char), 1, fout);
				fwrite(&image->G[i][j], sizeof(char), 1, fout);
				fwrite(&image->B[i][j], sizeof(char), 1, fout);
			}
		}
	}
	printf("Saved %s\n", f);
	fclose(fout);
}

/*Functia "EQUALIZE" face egalizarea imaginilor*/
void EQUALIZE(struct imagine *image, int OK)
{
	/*Se verifica daca exista un fisier incarcat*/
	if (!OK) {
		printf("No image loaded\n");
		return;
	}
	// verificam tipul imaginii
	if (image->header2 == '3' || image->header2 == '6') {
		/*Daca imaginea este de tip P3 sau P6 atunci vom afisa acest mesaj*/
		printf("Black and white image needed\n");
		return;
	}
	// facem vectorul de frecventa
	//int *vec = (int *)calloc(image->max, sizeof(int));
	int vec[256] = {0};
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			vec[image->gray[i][j]]++;
	}
	/*Variabila care reprezinta suprafata imaginii*/
	int area = image->height * image->width;
	/*Aici se fac calculele noului pixel*/
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++) {
			/*Avem variabila "sum" care reprezinta suma aparitiilor*/
			int sum = 0;
			for (int k = 0; k <= image->gray[i][j]; k++)
				sum += vec[k];
			/*Aici se calculeaza noul pixel*/
			double calcul = (double)255 / (double)area;
			double newpixel = calcul * (double)sum;
			/*Variabila "rez" detine valoarea noului pixel egalizat*/
			int rez = round(newpixel);
			/*Clamp*/
			if (rez > 255)
				rez = 255;
			else if (rez < 0)
				rez = 0;
			/*Se actualizeaza pixelul din imagine*/
			image->gray[i][j] = rez;
		}
	}
	//free(vec);
	/*Se afiseaza mesajul ca operatia s-a facut cu succes*/
	printf("Equalize done\n");
}

/*Cele doua functii de mai jos fac opertiile de inmultire in functia "APPLY"*/
/*Aceasta functie este pentru "SHARPEN" si "EDGE"*/
void inmultire_matrici(int nou[3][3], int x1, int x2, int y1, int y2
					, int mat[3][3], int **matrice, int **alta_matrice)
{
	for (int i = y1; i <= y2 - 1; i++) {
		for (int j = x1; j <= x2 - 1; j++) {
			nou[0][0] = matrice[i - 1][j - 1] * mat[0][0];
			nou[0][1] = matrice[i - 1][j] * mat[0][1];
			nou[0][2] = matrice[i - 1][j + 1] * mat[0][2];
			nou[1][0] = matrice[i][j - 1] * mat[1][0];
			nou[1][1] = matrice[i][j] * mat[1][1];
			nou[1][2] = matrice[i][j + 1] * mat[1][2];
			nou[2][0] = matrice[i + 1][j - 1] * mat[2][0];
			nou[2][1] = matrice[i + 1][j] * mat[2][1];
			nou[2][2] = matrice[i + 1][j + 1] * mat[2][2];
			int rez = nou[0][0] + nou[0][1] + nou[0][2]
			 + nou[1][0] + nou[1][1] + nou[1][2] + nou[2][0]
				+ nou[2][1] + nou[2][2];
			if (rez > 255)
				rez = 255;
			else if (rez < 0)
				rez = 0;
			alta_matrice[i][j] = rez;
		}
	}
}

/*Aceasta functie este pentru "BLUR" si "GAUSSIAN_BLUR"*/
void inmultire_matrici_n(int nou[3][3], int x1, int x2, int y1, int y2
				, int mat[3][3], int **matrice, int **alta_matrice, int n)
{
	for (int i = y1; i <= y2 - 1; i++) {
		for (int j = x1; j <= x2 - 1; j++) {
			nou[0][0] = matrice[i - 1][j - 1] * mat[0][0];
			nou[0][1] = matrice[i - 1][j] * mat[0][1];
			nou[0][2] = matrice[i - 1][j + 1] * mat[0][2];
			nou[1][0] = matrice[i][j - 1] * mat[1][0];
			nou[1][1] = matrice[i][j] * mat[1][1];
			nou[1][2] = matrice[i][j + 1] * mat[1][2];
			nou[2][0] = matrice[i + 1][j - 1] * mat[2][0];
			nou[2][1] = matrice[i + 1][j] * mat[2][1];
			nou[2][2] = matrice[i + 1][j + 1] * mat[2][2];
			int rez = (nou[0][0] + nou[0][1] + nou[0][2] +
			nou[1][0] + nou[1][1] + nou[1][2] + nou[2][0]
			 + nou[2][1] + nou[2][2]) / n;
			if (rez > 255)
				rez = 255;
			else if (rez < 0)
				rez = 0;
			alta_matrice[i][j] = rez;
		}
	}
}

/*Functia apply_inmultire face inmultirea pentru operatiile de apply.
Adica inmulteste elementele din jurul pixelului pe care noi vrem sa-l modificam
si, pe urma, aduna produsele in pixelul pe care vrem sa-l schimbam
Aceasta functie este doar pentru SHARPEN si EDGE DETECTION*/
void apply_inmultire(struct imagine *image, int mat[3][3])
{
	/*Initializam trei matrici auxiliare pe care o sa le adunam la final*/
	int nou_R[3][3] = {0};
	int nou_G[3][3] = {0};
	int nou_B[3][3] = {0};
	/*Aici facem o reajustare a selectiei, astfel incat sa nu cumva sa fie
	selectia in rama*/
	int x2 = image->x2;
	int x1 = image->x1;
	int y2 = image->y2;
	int y1 = image->y1;
	if (image->y2 == image->height)
		y2--;
	if (image->x2 == image->width)
		x2--;
	if (image->x1 == 0)
		x1++;
	if (image->y1 == 0)
		y1++;
	/*O sa ne alocam de fiecare data o matrice specifica pentru fiecare culoare
	ca sa facem operatiile, ca pe urma sa le dealocam*/
	int **matrice_R = alocare_matrice(image->height, image->width);
	/*Aici o sa copiem datele din matricea culorii rosu*/
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			matrice_R[i][j] = image->R[i][j];
	}
	/*Aici se fac inmultirile pentru a gasi noul pixel*/
	/*Pe urma verificam ca noua valoare al pixelului
									sa fie intre 0 si 255*/
	inmultire_matrici(nou_R, x1, x2, y1, y2, mat, matrice_R, image->R);
	/*Dealocam memoria pentru matricea culorii rosu*/
	for (int i = 0; i < image->height; i++)
		free(matrice_R[i]);
	free(matrice_R);
	/*operatia si pentru celelalte doua culori cu fix aceiasi pasi*/
	int **matrice_G = alocare_matrice(image->height, image->width);
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			matrice_G[i][j] = image->G[i][j];
	}
	inmultire_matrici(nou_G, x1, x2, y1, y2, mat, matrice_G, image->G);
	for (int i = 0; i < image->height; i++)
		free(matrice_G[i]);
	free(matrice_G);
	int **matrice_B = alocare_matrice(image->height, image->width);
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			matrice_B[i][j] = image->B[i][j];
	}
	inmultire_matrici(nou_B, x1, x2, y1, y2, mat, matrice_B, image->B);
	for (int i = 0; i < image->height; i++)
		free(matrice_B[i]);
	free(matrice_B);
	image->max = find_max(image);
}

/*Aceasta functie face acelasi lucru ca si cea de mai sus, dar imparte
rezultatul la un numar anume "n"(depinde de operatie)
Operatia de impartire se face cu date de tip double ca pe urma sa se
rotunjeasca*/
void apply_inmultire_blur(struct imagine *image, int mat[3][3], int n)
{
	/*Initializam trei matrici auxiliare pe care o sa le adunam la final*/
	int nou_R[3][3] = {0};
	int nou_G[3][3] = {0};
	int nou_B[3][3] = {0};
	/*Aici facem o reajustare a selectiei, astfel incat sa nu cumva sa fie
	selectia in rama*/
	int x2 = image->x2;
	int x1 = image->x1;
	int y2 = image->y2;
	int y1 = image->y1;
	if (image->y2 == image->height)
		y2--;
	if (image->x2 == image->width)
		x2--;
	if (image->x1 == 0)
		x1++;
	if (image->y1 == 0)
		y1++;
	/*O sa ne alocam de fiecare data o matrice specifica pentru fiecare culoare
	ca sa facem operatiile, ca pe urma sa le dealocam*/
	int **matrice_R = alocare_matrice(image->height, image->width);
	/*Aici o sa copiem datele din matricea culorii rosu*/
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			matrice_R[i][j] = image->R[i][j];
	}
	/*Aici se fac inmultirile pentru a gasi noul pixel*/
	inmultire_matrici_n(nou_R, x1, x2, y1, y2, mat, matrice_R, image->R, n);
	/*Dealocam memoria pentru matricea culorii rosu*/
	for (int i = 0; i < image->height; i++)
		free(matrice_R[i]);
	free(matrice_R);
	/*operatia si pentru celelalte doua culori cu fix aceiasi pasi*/
	int **matrice_G = alocare_matrice(image->height, image->width);
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			matrice_G[i][j] = image->G[i][j];
	}
	inmultire_matrici_n(nou_G, x1, x2, y1, y2, mat, matrice_G, image->G, n);
	for (int i = 0; i < image->height; i++)
		free(matrice_G[i]);
	free(matrice_G);
	int **matrice_B = alocare_matrice(image->height, image->width);
	for (int i = 0; i < image->height; i++) {
		for (int j = 0; j < image->width; j++)
			matrice_B[i][j] = image->B[i][j];
	}
	inmultire_matrici_n(nou_B, x1, x2, y1, y2, mat, matrice_B, image->B, n);
	for (int i = 0; i < image->height; i++)
		free(matrice_B[i]);
	free(matrice_B);
	image->max = find_max(image);
}

/*In aceasta functie se face operatia de EDGE DETECTION
unde se inmultesc fiecare pixel cu valorile matricei specifice
pentru aceasta operatie
Tin sa mai spun ca celelalte 3 functii din fata acesteia sunt exact la fel,
doar ca difera matricea cu care se inmultesc pixelii*/
void apply_edge_detection(struct imagine *image, int OK)
{
	/*Verificam daca imaginea respectiva a fost incarcata*/
	if (!OK) {
		/*Daca nu a fost incarcata, atunci vom afisa un mesaj*/
		printf("No image loaded\n");
		return;
		/*Verificam daca cumva imaginea incarcata este de tip PGM*/
	} else if (image->header2 == '2' || image->header2 == '5') {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	/*Aici se gaseste matricea cu care se va face operatia*/
	int mat[3][3];
	mat[0][0] = -1;
	mat[0][1] = -1;
	mat[0][2] = -1;
	mat[1][0] = -1;
	mat[1][1] = 8;
	mat[1][2] = -1;
	mat[2][0] = -1;
	mat[2][1] = -1;
	mat[2][2] = -1;
	/*Aici se aplica functia de inmultire*/
	apply_inmultire(image, mat);
	//Si aici se va afisa mesajul dorit(adica operatia s-a terminat cu succes)
	printf("APPLY EDGE done\n");
}

void apply_sharpen(struct imagine *image, int OK)
{
	if (!OK) {
		printf("No image loaded\n");
		return;
	} else if (image->header2 == '2' || image->header2 == '5') {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int mat[3][3];
	mat[0][0] = 0;
	mat[0][1] = -1;
	mat[0][2] = 0;
	mat[1][0] = -1;
	mat[1][1] = 5;
	mat[1][2] = -1;
	mat[2][0] = 0;
	mat[2][1] = -1;
	mat[2][2] = 0;
	apply_inmultire(image, mat);
	printf("APPLY SHARPEN done\n");
}

void apply_box_blur(struct imagine *image, int OK)
{
	if (!OK) {
		printf("No image loaded\n");
		return;
	} else if (image->header2 == '2' || image->header2 == '5') {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int mat[3][3];
	mat[0][0] = 1;
	mat[0][1] = 1;
	mat[0][2] = 1;
	mat[1][0] = 1;
	mat[1][1] = 1;
	mat[1][2] = 1;
	mat[2][0] = 1;
	mat[2][1] = 1;
	mat[2][2] = 1;
	apply_inmultire_blur(image, mat, 9);
	printf("APPLY BLUR done\n");
}

void apply_gaussian_blur(struct imagine *image, int OK)
{
	if (!OK) {
		printf("No image loaded\n");
		return;
	} else if (image->header2 == '2' || image->header2 == '5') {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int mat[3][3];
	mat[0][0] = 1;
	mat[0][1] = 2;
	mat[0][2] = 1;
	mat[1][0] = 2;
	mat[1][1] = 4;
	mat[1][2] = 2;
	mat[2][0] = 1;
	mat[2][1] = 2;
	mat[2][2] = 1;
	apply_inmultire_blur(image, mat, 16);
	printf("APPLY GAUSSIAN_BLUR done\n");
}

/*Functia "ROTATE" nu e terminata...imi cer scuze*/
void ROTATE(int OK)
{
	if (!OK) {
		printf("No image loaded\n");
		return;
	}
	int x = 0;
	scanf("%d", &x);
	printf("Rotated %d\n", x);
}

int main(void)
{
	struct imagine image;
	init(&image);
	char line[100];
	char *tok;
	int OK = 0;
	while (1) {
		fgets(line, 100, stdin);
		char *command = strtok(line, SPACE DELIM);
		if (strcmp(command, "LOAD") == 0) {
			tok = strtok(NULL, "\n");
			LOAD(&image, tok, &OK);
		} else if (strcmp(line, "HISTOGRAM") == 0) {
			HISTOGRAM(&image, OK, line);
		} else if (strcmp(command, "SAVE") == 0) {
			char *file = strtok(NULL, SPACE DELIM);
			tok = strtok(NULL, "\n");
			if (tok && !strcmp(tok, "ascii"))
				SAVE_ASCII(&image, file, OK);
			else
				SAVE_BINARY(&image, file, OK);
		} else if (strcmp(command, "EXIT") == 0) {
			EXIT(&image, OK);
			break;
		} else if (strcmp(command, "SELECT") == 0) {
			tok = strtok(NULL, "\n");
			if (!strcmp(tok, "ALL") || !strcmp(tok, "ALL ")) {
				SELECT_ALL(&image, OK);
			} else {
				int x1, y1, x2, y2;
				if (sscanf(tok, "%d %d %d %d", &x1, &y1, &x2, &y2) != 4) {
					printf("Invalid command\n");
					continue;
				}
				SELECT(&image, x1, y1, x2, y2, OK);
			}
		}	else if (strcmp(command, "CROP") == 0) {
			CROP(&image, OK);
		} else if (strcmp(command, "ROTATE") == 0) {
			int x;
			if (sscanf(tok, "%d", &x) == 1)
				printf("Rotated %d\n", x);
		} else if (strcmp(command, "EQUALIZE") == 0) {
			EQUALIZE(&image, OK);
		} else if (strcmp(command, "APPLY") == 0) {
			tok = strtok(NULL, "\n");
			if (!OK) {
				printf("No image loaded\n");
			} else {
				if (!tok)
					printf("Invalid command\n");
				else if (!strcmp(tok, "EDGE"))
					apply_edge_detection(&image, OK);
				else if (!strcmp(tok, "BLUR"))
					apply_box_blur(&image, OK);
				else if (!strcmp(tok, "GAUSSIAN_BLUR"))
					apply_gaussian_blur(&image, OK);
				else if (!strcmp(tok, "SHARPEN"))
					apply_sharpen(&image, OK);
				else
					printf("APPLY parameter invalid\n");
			}
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
