/*
    Fichier smtp.c
    Auteur Bernard Chardonneau

    Logiciel libre, droits d'utilisation préséen françs
    dans le fichier : licence-fr.txt

    Traductions des droits d'utilisation dans les fichiers :
    licence-de.txt , licence-en.txt , licence-es.txt ,
    licence-it.txt , licence-nl.txt , licence-pt.txt ,
    licence-eo.txt , licence-eo-utf.txt

    Droits d'utilisation élement sur la page web :
    http://libremail.tuxfamily.org/voir.php?page=droits


    Bibliothèe de fonctions permettant
    d'envoyer des mails au serveur smtp
*/

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include "buflect.h"
#include "smtp.h"
#include "base64.h"


// #define DEBUG

#define defaut_port_smtp  25  // port utiliséar déut par les serveurs smtp
#define timeout_connect    5  // attente maximum en secondes àa connexion

#define octet      unsigned char


/* variable globale au source
   (pour éter des tonnes de passages de paramèes) */

static int smtp = -1;         // descripteur pour dialoguer avec le serveur



/* connexion au serveur smtp indiquéans le fichier de configuration */

int connect_smtp (char *infoserveur)
{
    octet *carinfo;       // caractè de infoserveur
    char  serv_smtp [80]; // nom du serveur smtp utilisé   int   port;           // numé du port smtp utilisé   FILE  *fich_auth;     // descripteur du fichier d'authentification smtp
    char  bufin [80];     // buffer pour lire le fichier d'authentification
    char  bufw [80];      // buffer d'envoi d'une ligne ou requê smtp
    int   caract;         // caractè du fichier d'authentification smtp
    int   retour;         // code de retour de la fonction


    // initialisation du numé de port par déut
    port = defaut_port_smtp;

    // si la ligne smtp de fichier de configuration fait
    // rérence àn fichier d'authentification smtp
    if (*infoserveur == '>')
    {
        // se positionner sur le nom de ce fichier
        carinfo = infoserveur + 1;

        while (*carinfo == ' ' || *carinfo == '\t')
            carinfo ++;

        fich_auth = fopen (carinfo, "r");

        if (fich_auth)
        {
            // recupér le nom du serveur smtp
            fgets (serv_smtp, 80, fich_auth);
            serv_smtp [strlen (serv_smtp) - 1] = '\0';

            // mériser les autres lignes particuliès du fichier
            // d'authentification smtp (on s'arrê sur une ligne vide)
            while (fgets (bufin, 80, fich_auth) && *bufin != '\n')
            {
                bufin [strlen (bufin) - 1] = '\0';

                // numé de port spéfié
                if (startligne (bufin, "port"))
                {
                    // répér le numé du port
                    carinfo = bufin + 4;

                    while (*carinfo == ' ' || *carinfo == '\t')
                        carinfo++;

                    port = atoi (carinfo);

                    // le véfier sommairement
                    if (!port)
                    {
                        // "Numé de port smtp invalide"
                        affiche_err ("PORT_SMTP_INVALIDE");
                        retour = 0;
                    }
                }
            }

            // tentative de connexion au serveur smtp
            if (port)
                retour = connect_serv_smtp (serv_smtp, port);

            // si la connexion s'est bien déulé            if (retour)
            {
                // se positionner sur la premiè ligne
                // du dialogue d'authentification
                while (fgets (bufin, 80, fich_auth) && *bufin == '\n')
                    ;

                // dialogue d'authentification
                do
                {
                    // mise en forme de la ligne lue
                    bufin [strlen (bufin) - 1] = '\0';

                    // si ligne àncoder base 64
                    if (startligne (bufin, "b64"))
                    {
                        // l'encoder avant de l'envoyer
                        encode64 (bufin + 4, bufw, strlen (bufin + 4));
#ifdef DEBUG
                        puts (bufin);
#endif
                        envoie_smtp (bufw);
                    }
                    // sinon, envoyer directement la ligne
                    else
                        envoie_smtp (bufin);

                    // et lire la rénse du serveur
                    lire_smtp ();
                }
                // passage àa ligne suivante
                while (fgets (bufin, 80, fich_auth));
            }

            // libér le fichier d'authentification smtp
            fclose (fich_auth);
        }
        else
        {
            // message d'erreur
            // "Fichier %s non trouvé            aff_err_arg ("FICH_ABSENT", carinfo);
            retour = 0;
        }
    }
    // sinon simple connexion àn serveur smtp sans authentification
    else
    {
        // parcourir le nom du serveur smtp
        carinfo = infoserveur;

        while (*carinfo > ' ')
            carinfo ++;

        // si le nom du serveur smtp est suivi d'autres informations
        if (*carinfo)
        {
            // terminer le nom du serveur smtp
            *carinfo = '\0';

            // se positionner sur le numé de port éntuel
            do
                carinfo++;
            while (*carinfo == ' ' || *carinfo == '\t');
        }

        // si un numé de port semble prént
        if (*carinfo)
            // le répér
            port = atoi (carinfo);

        // si numé de port trouvéu si on a conservée port par déut
        if (port)
            // éblir la connexion smtp avec le port choisi
            retour = connect_serv_smtp (infoserveur, port);

        // sinon message d'erreur
        else
        {
            // "Numé de port smtp invalide"
            affiche_err ("PORT_SMTP_INVALIDE");
            retour = 0;
        }

        // si la connexion smtp s'est bien déulé        if (retour)
        {
            // gérer le message  helo nom_de_calculateur
            strcpy (bufw, "HELO ");
            gethostname (bufw + 5, sizeof (bufw) - 5);

            // identification de l'ordinateur connecté           envoie_smtp (bufw);
            lire_smtp ();
        }
    }

    // renvoyer le code de retour de la fonction
    return (retour);
}



/* connexion au serveur smtp */

int connect_serv_smtp (char *serveur, int port_smtp)
{
    struct hostent *HostEnt;      // description du host serveur
    struct sockaddr_in serv_addr; // addresse du serveur
    struct timeval st_timeout;    // structure pour fixer timeout àa connexion
    time_t avant, apres;          // pour déction sortie sur timeout


#ifdef DEBUG
    printf ("Appel de connect_serv_smtp (%s, %d)\n", serveur, port_smtp);
#endif

    // répétion adresse du serveur àartir de son nom
    HostEnt = gethostbyname (serveur);

    if (HostEnt == NULL)
    {
        // "Serveur smtp non trouvé        affiche_err ("SERV_SMTP_ABSENT");
        return (0);
    }

    // initialisation structure serv_adr
    memset (&serv_addr, 0, sizeof (serv_addr)); // init serv_addr

    memcpy (&serv_addr.sin_addr, HostEnt->h_addr, HostEnt->h_length);

    serv_addr.sin_port = htons (port_smtp);     // port smtp utilisé   serv_addr.sin_family = AF_INET;             // AF_*** : INET=internet

    // créion de la socket
    if ((smtp = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // "Problè créion socket client smtp"
        affiche_err ("CREAT_SOCKET_SMTP");
        return (0);
    }

    // on fixe un temps de rénse maximum àa connexion
    st_timeout.tv_sec  = timeout_connect;
    st_timeout.tv_usec = 0;

    if (setsockopt (smtp, SOL_SOCKET, SO_SNDTIMEO,
                          &st_timeout, sizeof (st_timeout)) < 0)
    {
        // "Problè pour fixer un timeout àa connexion : attente bloquante"
        affiche_err ("PB_FIX_TIMEOUT");
    }

    // on va mesurer le temps néssaire pour se connecter
    time (&avant);

    // requè de connexion
    if (connect (smtp, (struct sockaddr*) &serv_addr, sizeof (serv_addr)) < 0)
    {
        time (&apres);

        // afficher un message adapté       if (apres - avant >= timeout_connect)
            // "Problè demande de connexion : déi déssé            affiche_err ("PB_TIMEOUT_SERVEUR");
        else
        {
            // "Problè demande de connexion"
            // "peut êe due àne connexion Internet par proxy"
            affiche_err ("PB_ACCES_SERV-1");
            affiche_err ("PB_ACCES_SERV-2");
        }

        return (0);
    }
    else
    {
        // attendre la rénse du serveur smtp
        lire_smtp (smtp);
        return (1);
    }
}



/* teste si la ligne passéen paramèe commence par un mot cléarticulier */

int startligne (char *ligne, char *motcle)
{
    int i;

    i = 0;

    // on teste caractè par caractè en ignorant la casse
    while (motcle [i] && tolower (ligne [i]) == tolower (motcle [i]))
        i++;

    // le mot cléoit êe suivi d'un blanc dans la ligne lue
    return (ligne [i] == ' ');
}



/* Ecriture de donné
   envoie la chaine passéen paramèe suivie de \r\n
*/

void envoie_smtp (char *buffer)
{
#ifdef DEBUG
    putchar ('>');
    puts (buffer);
    fflush (stdout);
#endif

    write (smtp, buffer, strlen (buffer));
    write (smtp, "\r\n", 2);
}



/* Lecture d'une ligne de donné
   la lecture s'arrê sur un caractè de passage àa ligne
*/

void lire_smtp ()
{
    int posbuf;


    // initialisation
    posbuf = 0;

    // lecture
    do
        read (smtp, buf_lect + posbuf, 1);
    while (buf_lect [posbuf++] != '\n' && posbuf < sz_buflect);

#ifdef DEBUG
    buf_lect [posbuf - 1] = '\0';
    putchar ('<');
    puts (buf_lect);

    // si encodage base64, on affiche aussi la donnéen clair
    if (startligne (buf_lect, "334"))
    {
        decode64 (buf_lect + 4);
        puts (buf_lect);
    }

    fflush (stdout);
#endif
}



/* dénnexion propre du serveur smtp */

void deconnect_smtp ()
{
    // envoi du message de deconnexion
    envoie_smtp ("QUIT");

    // lecture de l'acquittement
    lire_smtp ();

    // fermeture de la connexion
    shutdown (smtp, 2);
    close (smtp);
}

