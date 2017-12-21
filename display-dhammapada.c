#ifndef VERSION
#define VERSION "0.20?"
#endif
static char c1[] = "     display-dhammapada V" VERSION " \n";
static char c2[] = " (C) January 1997-2003 by Ričardas Čepas (Richard Cepas) <rch@online.lt> and contributors. \n";
static char c3[] = " Copying policy: Public Domain.\n";
static char c4[] = " No warranty. \n";

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define max_string_length 278
#define mark_length 5
#ifndef DPPATHNAME
#define DPPATHNAME "/usr/local/lib/display-dhammapada/dhammapada-english-transl.txt:/usr/lib/display-dhammapada/dhammapada-english-transl.txt"
#endif
#ifndef DPPATHNAME_ALT
#define DPPATHNAME_ALT "/usr/local/lib/display-dhammapada/dhammapada-alternate.txt:/usr/lib/display-dhammapada/dhammapada-alternate.txt"
#endif
#ifndef DPPATHNAME_PL
#define DPPATHNAME_PL "/usr/local/lib/display-dhammapada/dhammapada-polish-transl.txt:/usr/lib/display-dhammapada/dhammapada-polish-transl.txt"
#endif
#ifndef NO_LOCALE
#include <locale.h>
#include <langinfo.h>
#include <iconv.h>
iconv_t iconvh = (iconv_t) -1;
char *outcharset;
#endif


static const char dp_pathname[] = DPPATHNAME ":dhammapada-english-transl.txt";
static const char c6[] = "                                                                                  ";
static const char dp_pathname_alt[] = DPPATHNAME_ALT ":dhammapada-alternate.txt";
static const char c7[] = "                                                                                  ";
static const char dp_pathname_pl[] = DPPATHNAME_PL ":dhammapada-polish-transl.txt";
const char bad_args[] = "Bad arguments";
const char IO_err[] = "I/O error";
const char cannot_open[] = "    -- cannot open any of the files.";
const char too_long_verse[] = " file format error ?";
const char unexp_EOF[] = "unexpected end of file.";


const int max_strings_in_verse = 20;

/*const long min_all_verses_size=50; */
static struct
  {
       char begin[mark_length], end[mark_length];
  }
marks[] =
  {
       {"1.","2"},{"2","4"},{"4","5"},{"5","6"},{"6","8"},
       {"8","10"},{"10","12"},{"12","14"},{"14","16"},{"16","18"},
       {"18","20"},{"2.","22"},{"22","24"},{"24","26"},{"26","28"},
       {"28","30"},{"30","32"},{"3.","33"},{"33","35"},{"35","36"},
       {"36","37"},{"37","39"},{"39","41"},{"41","43"},{"4.","45"},
       {"45","46"},{"46","47"},{"47","48"},{"48","49"},{"49","50"},{"50","52"},
       {"52","53"},{"53","54"},{"54","55"},{"55","56"},{"56","57"},
       {"57","59"},{"5.","60"},{"60","61"},{"61","62"},{"62","63"},
       {"63","65"},{"65","66"},{"66","68"},{"68","69"},{"69","70"},{"70","71"},
       {"71","72"},{"72","75"},{"6.","76"},{"76","77"},{"77","78"},
       {"78","79"},{"79","80"},{"80","81"},{"81","82"},{"82","83"},
       {"83","84"},{"84","86"},{"86","88"},{"88","89"},{"7.","90"},
       {"90","91"},{"91","92"},{"92","93"},{"93","94"},{"94","95"},
       {"95","96"},{"96","97"},{"97","98"},{"98","99"},{"8.","100"},
       {"100","101"},{"101","102"},{"102","103"},{"103","105"},{"105","106"},
       {"106","107"},{"107","108"},{"108","109"},{"109","110"},{"110","111"},
       {"111","112"},{"112","113"},{"113","114"},{"114","115"},{"9.","116"},
       {"116","118"},{"118","120"},{"120","122"},{"122","124"},{"124","125"},
       {"125","126"},{"126","128"},{"10.","130"},{"130","132"},{"132","133"},
       {"133","134"},{"134","135"},{"135","136"},{"136","140"},{"140","142"},
       {"142","143"},{"143","144"},{"144","145"},{"11.","146"},{"146","147"},
       {"147","148"},{"148","149"},{"149","150"},{"150","151"},{"151","152"},
       {"152","154"},{"154","155"},{"155","156"},{"12.","157"},{"157","158"},
       {"158","159"},{"159","160"},{"160","161"},{"161","162"},{"162","163"},
       {"163","164"},{"164","165"},{"165","166"},{"13.","167"},{"167","168"},
       {"168","169"},{"169","170"},{"170","171"},{"171","172"},{"172","173"},
       {"173","174"},{"174","175"},{"175","176"},{"176","177"},{"177","178"},{"14.","179"},
       {"179","180"},{"180","181"},{"181","182"},{"182","183"},{"183","184"},{"184","185"},
       {"185","187"},{"187","192"},{"192","193"},{"193","194"},{"194","196"},
       {"15.","197"},{"197","198"},{"198","199"},{"199","200"},{"200","201"},
       {"201","202"},{"202","204"},{"204","205"},{"205","207"},{"207","208"},
       {"16.","209"},{"209","211"},{"211","212"},{"212","213"},{"213","214"},
       {"214","215"},{"215","216"},{"216","217"},{"217","218"},{"218","220"},
       {"17.","221"},{"221","222"},{"222","223"},{"223","224"},{"224","225"},
       {"225","226"},{"226","227"},{"227","228"},{"228","230"},{"230","231"},
       {"231","232"},{"232","233"},{"233","234"},{"18.","236"},{"236","238"},
       {"238","239"},{"239","240"},{"240","241"},{"241","242"},{"242","243"},
       {"243","245"},{"245","248"},{"248","250"},{"250","251"},{"251","252"},
       {"252","253"},{"253","254"},{"254","255"},{"19.","257"},{"257","258"},
       {"258","259"},{"259","261"},{"261","263"},{"263","265"},{"265","267"},
       {"267","269"},{"269","270"},{"270","272"},{"20.","273"},{"273","274"},
       {"274","275"},{"275","276"},{"276","277"},{"277","278"},{"278","279"},
       {"279","280"},{"280","281"},{"281","282"},{"282","283"},{"283","284"},
       {"284","285"},{"285","286"},{"286","287"},{"287","289"},{"21.","290"},
       {"290","291"},{"291","293"},{"293","295"},{"295","296"},{"296","297"},
       {"297","298"},{"298","299"},{"299","300"},{"300","301"},{"301","302"},
       {"302","303"},{"303","304"},{"304","305"},{"22.","306"},{"306","307"},
       {"307","308"},{"308","309"},{"309","310"},{"310","311"},{"311","312"},
       {"312","313"},{"313","314"},{"314","315"},{"315","316"},{"316","317"},
       {"317","319"},{"23.","320"},{"320","321"},{"321","322"},{"322","323"},
       {"323","324"},{"324","325"},{"325","326"},{"326","327"},{"327","329"},{"329","331"},
       {"331","332"},{"332","333"},{"24.","334"},{"334","336"},{"336","337"},
       {"337","338"},{"338","339"},{"339","340"},{"340","341"},{"341","342"},
       {"342","343"},{"343","344"},{"344","346"},{"346","347"},
       {"347","348"},{"348","351"},{"351","352"},{"352","353"},{"353","354"},
       {"354","355"},{"355","356"},{"356","357"},{"357","358"},{"358","359"},
       {"25.","360"},{"360","361"},{"361","362"},{"362","363"},{"363","364"},
       {"364","365"},{"365","366"},{"366","367"},{"367","368"},{"368","369"},
       {"369","370"},{"370","371"},{"371","372"},{"372","373"},{"373","374"},{"374","376"},
       {"376","377"},{"377","378"},{"378","379"},{"379","380"},{"380","381"},{"381","382"},
       {"26.","383"},{"383","384"},{"384","385"},{"385","386"},{"386","387"},
       {"387","388"},{"388","389"},{"389\n","390"},{"390\n","391"},{"391","392"},
       {"392","393"},{"393\n","394"},{"394","395"},{"395","396"},{"396\n","397"},
       {"397","398"},{"398","399"},{"399","400"},{"400","401"},{"401","402"},
       {"402","403"},{"403","404"},{"404","405"},{"405","406"},{"406","407"},
       {"407","408"},{"408","409"},{"409","410"},{"410","411"},{"411","412"},
       {"412","413"},{"413","414"},{"414","415"},{"415","416"},{"416","417"},
       {"417","418"},{"418","419"},{"419","420"},{"420","421"},{"421","422"},
       {"422","423"}
  };

void
Error (const char *message)
{
     fprintf (stderr, "dp: %s \n", message);
     exit (1);
}

void
Find_begin_of_verses (FILE * dp)
{
     char s[max_string_length];

     rewind (dp);
     do
          if (fgets (s, max_string_length, dp) == NULL)
               if (feof (dp))
                    Error (unexp_EOF);
               else
                    Error (IO_err);
     while (strstr (s, "---------------------------------------------") == NULL);
     do
          if (fgets (s, max_string_length, dp) == NULL)
               if (feof (dp))
                    Error (unexp_EOF);
               else
                    Error (IO_err);
     while (strstr (s, "---------------------------------------------") == NULL);
}
void
Find_verse (FILE * dp, char *begin_mark)
{
     char s[max_string_length];

     do
          if (fgets (s, max_string_length, dp) == NULL)
               if (feof (dp))
                    Error (unexp_EOF);
               else
                    Error (IO_err);
     while (strstr (s, begin_mark) == NULL);
}

void
Help ()
{
     printf ("%s Displays a random verse \n", c1);
     printf ("from John Richards's translation:\n%s \nor from F. Max Muller's translation:\n%s\n", dp_pathname, dp_pathname_alt);
     printf (" Arguments: \n"
	     "           -r               Use John Richard's translation (default)\n"
	     "           -m               Use F. Max Muller's translation\n"
	     "           -b               Display the same verse(s) from both\n"
             "           -pl              Use Polish language translation\n"
	     "           <number>         specify the number of the verse(s) to display \n"
	     "           all              display all file at once \n");
     printf ("%s%s%s", c2, c3, c4);
}

void
Print_verse (FILE * dp, char *end_mark)
{
     char s[max_string_length];
     int printed_strings_counter = 0;
#ifndef NO_LOCALE
     char s2[max_string_length*2];
     size_t conv, inleft, outleft;
     const char *inptr;
     char *outptr;
#endif

     do
       {
            if (fgets (s, max_string_length, dp) == NULL)
                 if (feof (dp))
                      Error (unexp_EOF);
                 else
                      Error (IO_err);
#ifndef NO_LOCALE
	    if (iconvh != (iconv_t) -1) {
	      strcpy(s2, s);
	      errno = 0;
	      inleft = strlen(s2);
	      outleft = sizeof(s)-1;
	      inptr = s2; outptr = s;
	      conv = iconv(iconvh, &inptr, &inleft, &outptr, &outleft);
	      if (conv  == (size_t) -1 || errno) {
		  fprintf(stderr, "Conversion to %s error %i: input is:\n%s", outcharset, errno, inptr);
		  exit (1);
	      }
	      *outptr = '\0';
	    }
#endif
            printf (" %s ", s);
            printed_strings_counter++;
            if (printed_strings_counter > max_strings_in_verse)
                 Error (too_long_verse);
       }
     while (strstr (s, end_mark) == NULL);
     printf ("\n");
}

void
Get_and_print_verse (FILE * dp, int index)
{
     Find_begin_of_verses (dp);
     if (index >= sizeof marks / sizeof marks[0])
          index = sizeof marks / sizeof marks[0] - 1;
     Find_verse (dp, marks[index].begin);
     Print_verse (dp, marks[index].end);
}

void
Get_and_print_verse_number_n (FILE * dp, int number)
{
  int index;

  for (index = 0; index < sizeof marks / sizeof marks[0] - 1; index++)
    if (number <= atoi (marks[index].end))
      break;
  Get_and_print_verse (dp, index);
}

int
Random_index ()
{
     int i, random_index;

     srand ((unsigned int) time (NULL));
/*      min=0, max=numb.of elem.-1+.999 */
     random_index = (int) ((double) (sizeof marks / sizeof marks[0]) * (i = rand ()) / (RAND_MAX + 1.0));
     return (random_index);
}

void Filename_num (char* filename, const char* filename_list, int num)
{
	int i;
	for (i = 1; i < num; i++) {	
		while (*filename_list != ':' && *filename_list != '\0') 
			filename_list++;
		if (*filename_list == '\0') {
			*filename = '\0';
			return;
		}
		filename_list++;
	}
	while (*filename_list != ':' && *filename_list != '\0')
		*filename++ = *filename_list++;
	*filename = '\0';
}

int
main (int argc, char *argv[])
{
     FILE *dp;
     int index = -1;
     char *stop_char_ptr;
     char dp_filename [200];
     enum {MULLER, RICHARDS, BOTH, POLISH} dhammapada_version = RICHARDS;
     int filename_number;
     int is_all = 0;
#ifndef NO_LOCALE
     char *locale;
     
     locale = setlocale(LC_ALL, "");
     if (locale != NULL && locale[0] == 'p' && locale[1] == 'l')
       dhammapada_version = POLISH;
     outcharset = nl_langinfo(CODESET);
     if ((iconvh = iconv_open(outcharset, "utf-8")) == (iconv_t) -1)
     if ((iconvh = iconv_open(outcharset, "UTF-8")) == (iconv_t) -1)
     if ((iconvh = iconv_open(outcharset, "utf8")) == (iconv_t) -1)
       1;
#endif

     /* Split the options and set the booleans */
     if (argc > 1) {
	     if (strcmp (argv[1], "-b") == 0) dhammapada_version = BOTH;
	     else if (strcmp (argv[1], "-r") == 0) dhammapada_version = RICHARDS;
	     else if (strcmp (argv[1], "-m") == 0) dhammapada_version = MULLER;
	     else if (strcmp (argv[1], "-pl") == 0) dhammapada_version = POLISH;
	     else if (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "-?") == 0) {
	        Help ();
		exit (0);
	     }
	     else if (!(is_all = ((strcmp (argv[1], "all") == 0)))) {
		errno = 0;
		index = strtol (argv[1], &stop_char_ptr, 0);
		if (errno == ERANGE || *stop_char_ptr != '\0') { 
			index = -1;
			Help ();
			Error (bad_args);
		}
	     }
	     if (argc > 2) {
		if (index == -1 || ! is_all) {
	     		if (!(is_all = ((strcmp (argv[2], "all") == 0)))) {
				errno = 0;
				index = strtol (argv[2], &stop_char_ptr, 0);
				if (errno == ERANGE || *stop_char_ptr != '\0') 
					index = -1;
			}
		}
		else {
			Help ();
			Error (bad_args);
		}
	     }
	}
open_file:
     /* Open the file (Richards if both) */
     dp = NULL;
     dp_filename[0] = ' ';
     filename_number = 1;
     while (dp == NULL) {
	     
	     if (dhammapada_version == MULLER) {
	     	Filename_num (dp_filename, dp_pathname_alt, filename_number);
	     }
	     else if (dhammapada_version == POLISH) {
	     	Filename_num (dp_filename, dp_pathname_pl, filename_number);
             }
             else Filename_num (dp_filename, dp_pathname, filename_number);
	     
	     if (dp_filename[0] == '\0') {
		     if (dhammapada_version == MULLER) 
			     fprintf (stderr, "%s\n", dp_pathname_alt);
		     else if (dhammapada_version == POLISH)  
			     fprintf (stderr, "%s\n", dp_pathname_pl);
		     else
			fprintf (stderr, "%s\n", dp_pathname);	     
		     Error (cannot_open);
	     }
	     
	     dp = fopen (dp_filename, "rt");
	     filename_number++;
     }
     /* Print the verse(s) */
     if (is_all) 
     	for (index = 0; index < sizeof marks / sizeof marks[0]; index++)
		Get_and_print_verse (dp, index);
     else if (index != -1)
	Get_and_print_verse_number_n (dp, index);
     else 
        Get_and_print_verse (dp, Random_index ());
     /* Close the file */
     fclose (dp);
     /* If both, set dhammapada_version to MULLER and goto open_file */
     if (dhammapada_version == BOTH) {
	     dhammapada_version = MULLER;
	     goto open_file;
     }
#ifndef NO_LOCALE
     if (iconvh != (iconv_t) -1) iconv_close(iconvh);
#endif
     exit (0); 
}
