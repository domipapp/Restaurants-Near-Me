#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef struct etterem{
    unsigned int id;
    char nev[50];
    char cim[50];
    char konyha[30];
    double eszaki;/* északi keleti koordináták*/
    double keleti;
    double minosites;/*5.0-1.0*/
    char arkat[4];/*$ $$ $$$*/
    char terasz[4];/*igen/nem*/
    struct etterem *next;/*következő étterem*/
    struct etterem *prev;/*előző étterem*/
    struct asztal *head;/*asztalok listájára mutató pointer*/
}etterem;

typedef struct asztal{
    unsigned int id;/*ez megegyezik az etterem.id -val*/
    int ferohely;/*asztal ülőhelyeinek száma*/
    int szabad;/*szabad asztalok száma*/
    struct asztal *next;/*az étterem asztalainak következő elemére mutat*/
}asztal;

typedef struct user{
    int sugar;
    double eszaki;/* északi keleti koordináták*/
    double keleti;
    char konyha1[20];
    char konyha2[20];/*csak akkor kerül bele adat ha több mint egy van megadva, egyébként csupa '\0'*/
    char konyha3[20];
    double minosites;
    char arkat[4];/*$ $$ $$$ felsőhatár*/
    char terasz[5];/*igen/nem*/
    int fo;/*legalább ennyi fős asztal kell*/
}user;

etterem* create_etterem(void){
    return calloc(1,sizeof(etterem)); 
}

asztal* create_asztal(void){
    return calloc(1,sizeof(asztal)); 
}

user* create_user(void){
    return calloc(1,sizeof(user));
}

etterem* insert_etterem(etterem* insert, etterem* head){
    if(head == NULL)
        return insert;//Új head
    if(head->next == NULL){
        if(head->minosites > insert->minosites){//Ha a lista egy elemes elé vagy mögé kell beszúrni
            head->next = insert;
            insert->prev = head;}
        else{
            insert->next = head;
            head->prev = insert;
            head = insert;}//Új head
        return head;
    }
    if(insert->minosites > head->minosites){///Ha a lista nem egy elemes elé vagy mögé kell beszúrni//lehet felesleges fáradt vagyok letesztelni
        insert->next = head;
        head->prev = insert;
        return insert;
    }
    etterem* current = head;
    while(1){//Megkeresi a az étterem minőség szerinti helyét a listában és elhelyezi
        if(current->next == NULL){
            current->next = insert;
            insert->prev = current;
            return head;
        }
        if(insert->minosites < current->minosites && insert->minosites > current->next->minosites){
            insert->next = current->next; 
            insert->prev = current;
            current->next = insert;
            insert->next->prev = insert;
            return head;
        }
        current = current->next;
    }
printf("Sikertelen étterem elem beillesztés!!!\n");
return NULL;//ERROR
}

void insert_asztal(asztal* insert, etterem* head){
    etterem* search = head;
    while(search != NULL && search->id != insert->id)//Megkeresi a megfelelő éttermet
        search = search->next;
    if(search == NULL){//nincs ilyen étterem
        free(insert);//fel kell szabadítani
        return;}
    asztal* end = search->head;
    if(end == NULL){
        search->head = insert;
        return;}
    while(end->next != NULL)//Megkeresi az aztal lista végét
        end = end->next;
    end->next = insert;

}

etterem* beolvas_etterem(etterem* head){
    FILE *ettermek = fopen("ettermek.txt", "r");
    char test;
    etterem* back = create_etterem();
    while(fscanf(ettermek, "%u;%[^;];%[^;];%[^;];%lf;%lf;%lf;%[^;];%s%c", &back->id, back->nev, back->cim, back->konyha, &back->eszaki, &back->keleti, &back->minosites, back->arkat, back->terasz,  &test) != EOF){
        if(test != '\n' && test != EOF && test != '\0'){
            printf("Nem megfelelő mennyiségű adat egy étteremhez .\n");
            free(back);
            return head;}//innentől kezdve elcsúszna a beolvasás, értelmetlen folytatni
        head = insert_etterem(back, head);
        back = create_etterem();
    }
    free(back);//While után marad egy üres elem
    if(fclose(ettermek) == -1)
        printf("Éttermek file nyitva!!!\n");
return head;
}

int beolvas_asztal(etterem* head){
    FILE *asztalok = fopen("asztalok.txt", "r");
    char test;
    int count = 0;
    asztal* back = create_asztal(); 
    while(fscanf(asztalok, "%d;%d;%d%c", &back->id, &back->ferohely, &back->szabad, &test) != EOF){
    if(test != '\n' && test != EOF && test != '\0')
        printf("Nem megfelelő mennyiségű adat egy asztalhoz\n");
    insert_asztal(back, head);
    back = create_asztal();
    count ++;
    }
    free(back);//While után marad egy üres elem
    if(count == 0)//sehol sincs asztal
        return 0;
    if(fclose(asztalok) == -1)
       printf("Asztalok file nyitva!!!\n");
return 1;
}

user* beolvas_user(void){
    FILE *user_fptr = fopen("user.txt", "r");
    user* back = create_user();
    char tmp[100];
    int a = fscanf(user_fptr, "%d;%lf;%lf;%[^;];%lf;%[^;];%[^;];%d", &back->sugar, &back->eszaki, &back->keleti, tmp, &back->minosites, back->arkat, back->terasz, &back->fo);//tmp-be kerül az összes konyha
    if(a == -1)//üres a fájl
        printf("A user.txt fájl üres!!!\n");
    if(a != 8)
        printf("Nem megfelelő mennyiségű user adat!\n");
    sscanf(tmp, "%s%s%s", back->konyha1, back->konyha2, back->konyha3);//Szétbontja a tmp-t konyhákra
    if(fclose(user_fptr) != 0)
        printf("User file nyitva!!!\n");
    return back;
}

etterem* pop_etterem(etterem* pop, etterem* head){
    asztal* end = pop->head;
    if(end == NULL){//nincs asztal
        printf("Nincs asztal a(z) %sban/ben.\n", pop->nev);
        if(pop->prev != NULL)
        pop->prev->next = pop->next;
        if(pop->next != NULL)
            pop->next->prev = pop->prev;
        free(pop);
        return head;}
    else if(pop == head)//Ha a felszabadítani kívánt elem a head akkor új head
        head = pop->next;
    else if(pop->next == NULL)//Ha a felszabadítani kívánt elem az utolsó akkor az előző lesz az új utolsó(előző a NULL-ra mutat)
        pop->prev->next = pop->next;
    else{//Ha a felszabadítani kívánt elem se nem első se nem utolsó
        pop->prev->next = pop->next;
        pop->next->prev = pop->prev;}
    free(pop);
    asztal* kov = end;
    while(end->next != NULL){//*Asztal lista felszabadítása
        if(kov != NULL)
            kov = kov->next;
        free(end);
        end = kov;
    }
    free(end);//While után marad egy felszabadítatlan asztal elem
return head;
}

etterem* search_for_bad(user* user, etterem* head){
    etterem* first_bad = head;
    etterem* kov;
    while(first_bad != NULL && user->minosites < first_bad->minosites)//Első rossz megkeresése
        first_bad = first_bad->next;
    while(first_bad != NULL){//Összes rossz felszabadítása
        kov = first_bad->next;
        head = pop_etterem(first_bad, head);
        first_bad = kov;
    }    
return head;    
}

double tavolsag(user* user, etterem* a){//Haversine formula
    double desz = (M_PI / 180) * (user->eszaki - a->eszaki); //szélességi és hosszúsági különbségek radiánban
    double dkh = (M_PI / 180) * (user->keleti - a->keleti);
    double uesz = (M_PI / 180) * (user->eszaki); //radiánba váltás
    double aesz = (M_PI / 180) * (a->eszaki);  //user északi szszélesség a északi szélesség
    double tmp = pow(sin(desz / 2), 2) +  pow(sin(dkh / 2), 2) *  cos(aesz) * cos(uesz); 
    return 6371 * 2 * asin(sqrt(tmp)) * 1000;//méterben 
}

etterem* compare(user* user, etterem* head){
    etterem* search = head;
    etterem* kov;
    asztal* ts = search->head;//table search -> ts
    int test = 0;
    while(search != NULL){
        ts = search->head;
        //Konyha, árkategória és távolság kritériumok megvizsgálása
        if((strcmp(search->konyha, user->konyha1) == 0 || strcmp(search->konyha, user->konyha2) == 0 || strcmp(search->konyha, user->konyha3) ==0) && (strlen(search->arkat) <= strlen(user->arkat)) && (tavolsag(user, search) <= user->sugar))
            test = 1;
        int a = strlen(user->terasz);
        int b = strlen(search->terasz);
        if(a == 4 && b == 3)//Terasz elvárások összehasonlítása
            test = 0;    
        while(ts != NULL && test == 1){//Megfelelő asztal elérhetőségének vizsgálata
            if((ts->ferohely >= user->fo) && (ts->szabad != 0))
                break;
            if(ts->next == NULL)
                test = 0;
            ts = ts->next;
        }
        if(test == 1){
            search = search->next;
            test = 0;
            continue;
        }
        kov = search->next;//Ha az étterem nem felel meg a kritériumoknak
        head = pop_etterem(search, head);
        search = kov;
    }
return head;
}

void print_etterem(user* user, etterem* head){
    asztal* ts = head->head;//table search = ts
    while(ts != NULL){//Megkeresi az első megfelelő asztalt és "elmenti"
            if((ts->ferohely >= user->fo) && (ts->szabad != 0))
                break;
            ts = ts->next;
        }
    if(ts != NULL)
    printf("%s\n%s\n%s\n%d\n%.2f méter\n\n", head->nev, head->konyha, head->cim, ts->ferohely, tavolsag(user, head));
    head = pop_etterem(head, head);
    if(head != NULL)//head == NULL az escape feltétel(pop_etterem NULL-al tér vissza ha nincs több elem a listában)
        print_etterem(user, head);
}

int main(){
    etterem* head_etterem = NULL;//A program a NULL-ból tudja hogy még nincs elem a listában
    head_etterem = beolvas_etterem(head_etterem);//etterem lista megepitve, file becsukva
    if(head_etterem == NULL){//nincs egy darab étterm sem a listában
        printf("Az ettermek.txt fájl üres!!!");//
        return 0;
    }
    int test = beolvas_asztal(head_etterem);//asztal lista megepitve, file becsukva
    user* user = beolvas_user();//user struktúra megépítve
    if(test == 0){
        user->minosites = 6;//sosincs olyan étterem amely megfelelne ennek a kritériumnak
        head_etterem = search_for_bad(user, head_etterem);//ez minden éttermet felszabadít így
        free(user);
        printf("Az asztalok.txt fájl üres!!!");
        return 0;
    }
    head_etterem = search_for_bad(user, head_etterem);//Ha semmi sem jó akkor null pointer a head
    if(head_etterem == NULL){
        printf("Nincs a kritériumainak megfelelő étterem. Csökkentse a minőségbeli elvárásait\n");//Üres a lista
        free(user);
        return 0;
    }
    head_etterem = compare(user, head_etterem);//ez után csak jók maradnak a listában
    if(head_etterem == NULL){
        printf("Nincs a kritériumainak megfelelő étterem. Csökkentse a valamely elvárását\n");//Üres a lista
        free(user);
        return 0;
    }
    print_etterem(user, head_etterem);//mindent kiír és a teljes listát felszabadítja
    free(user);
    return 0;
}