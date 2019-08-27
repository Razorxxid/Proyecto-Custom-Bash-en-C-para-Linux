#include <check.h>
#include "test_scommand.h"

#include <signal.h>
#include <assert.h>
#include <string.h> /* para strcmp */
#include <stdlib.h> /* para calloc */
#include <stdio.h> /* para sprintf */

#include "bstring/bstrlib.h"
#include "command.h"

#define MAX_LENGTH 257 /* no hay nada como un primo para molestar */

static scommand scmd = NULL; /* para armar scommand temporales */

/* Testeo precondiciones. 
 * Se espera que todos estos tests fallen con un assert(), son todas 
 * llamadas inválidas 
 */
START_TEST (test_destroy_null)
{
	scommand_destroy (NULL);
}
END_TEST


START_TEST (test_push_back_null)
{
	scommand_push_back (NULL, bfromcstr ("123"));
}
END_TEST

START_TEST (test_push_back_argument_null)
{
	scmd = scommand_new ();
	scommand_push_back (scmd, NULL);
	scommand_destroy (scmd); scmd = NULL;
}
END_TEST

START_TEST (test_pop_front_null)
{
	scommand_pop_front (NULL);
}
END_TEST

START_TEST (test_pop_front_empty)
{
	scmd = scommand_new();
	scommand_pop_front (scmd);
	scommand_destroy(scmd); scmd = NULL;
}
END_TEST

START_TEST (test_set_redir_in_null)
{
	scommand_set_redir_in (NULL, bfromcstr("123"));
}
END_TEST

START_TEST (test_set_redir_out_null)
{
	scommand_set_redir_out (NULL, bfromcstr("123"));
}
END_TEST

START_TEST (test_is_empty_null)
{
	scommand_is_empty (NULL);
}
END_TEST

START_TEST (test_length_null)
{
	scommand_length (NULL);
}
END_TEST

START_TEST (test_front_null)
{
	scommand_front (NULL);
}
END_TEST

START_TEST (test_front_empty)
{
	scmd = scommand_new();
	scommand_front (scmd);
	scommand_destroy(scmd); scmd = NULL;
}
END_TEST

START_TEST (test_get_redir_in_null)
{
	scommand_get_redir_in (NULL);
}
END_TEST

START_TEST (test_get_redir_out_null)
{
	scommand_get_redir_out (NULL);
}
END_TEST

START_TEST (test_to_string_null)
{
	scommand_to_string (NULL);
}
END_TEST


/* Crear y destruir */
START_TEST (test_new_destroy)
{
	scmd = scommand_new ();
	/* Verificamos que se pueda crear y destruir un scommand sin problemas */
	scommand_destroy (scmd); scmd = NULL;
}
END_TEST

START_TEST (test_new_is_empty)
{
	scmd = scommand_new ();
	/* Un comando recién creado debe ser vacío */
	fail_unless (scommand_is_empty (scmd), NULL);
	fail_unless (scommand_length (scmd) == 0, NULL);
	scommand_destroy (scmd); scmd = NULL;
}
END_TEST

/* Testeo de funcionalidad */

static void setup (void) {
	scmd = scommand_new ();
}

static void teardown (void) {
	if (scmd != NULL) {
		scommand_destroy (scmd);
		scmd = NULL;
	}
}

/* is_empty sea acorde a lo que agregamos y quitamos */
START_TEST (test_adding_emptying)
{
	unsigned int i = 0;
	for (i=0; i<MAX_LENGTH; i++) {
		fail_unless ((i==0) == scommand_is_empty (scmd), NULL);
		scommand_push_back (scmd, bfromcstr ("123"));
	}
	for (i=0; i<MAX_LENGTH; i++) {
		fail_unless (!scommand_is_empty(scmd), NULL);
		scommand_pop_front (scmd);
	}
	fail_unless (scommand_is_empty (scmd), NULL);
}
END_TEST

/* length sea acorde a lo que agregamos y quitamos */
START_TEST (test_adding_emptying_length)
{
	unsigned int i = 0;
	for (i=0; i<MAX_LENGTH; i++) {
		fail_unless (i == scommand_length (scmd), NULL);
		scommand_push_back (scmd, bfromcstr ("123"));
	}
	for (i=MAX_LENGTH; i>0; i--) {
		fail_unless (i == scommand_length (scmd), NULL);
		scommand_pop_front (scmd);
	}
	fail_unless (0 == scommand_length (scmd), NULL);
}
END_TEST

/* Meter por atrás y sacar por adelante, da la misma secuencia. */
START_TEST (test_fifo)
{
	unsigned int i = 0;
	bstring *strings = NULL;
	strings = calloc (MAX_LENGTH, sizeof(bstring));
	for (i=0; i<MAX_LENGTH; i++) {
		strings[i] = bformat ("%d", i);
	}
	/* strings = ["0","1", ..., "256"] */
	for (i=0; i<MAX_LENGTH; i++) {
		/* Copia antes de push: scommand se apropia */
		scommand_push_back (scmd, bstrcpy(strings[i]));
	}
	for (i=0; i<MAX_LENGTH; i++) {
		/* mismo string */
		fail_unless (biseq (scommand_front (scmd),strings[i]) == 1, NULL);
		bdestroy(strings[i]);
		scommand_pop_front (scmd);
	}
	free (strings);
}
END_TEST

/* hacer muchísimas veces front es lo mismo */
START_TEST (test_front_idempotent)
{
	unsigned int i = 0;
	scommand_push_back (scmd, bfromcstr ("123"));
	for (i=0; i<MAX_LENGTH; i++) {
		fail_unless (biseqcstr (scommand_front (scmd), "123") == 1, NULL);
	}
}
END_TEST

/* Si hay solo uno, entonces front=back */
START_TEST (test_front_is_back)
{
	scommand_push_back (scmd, bfromcstr ("123"));
	fail_unless (biseqcstr (scommand_front (scmd), "123") == 1, NULL);
}
END_TEST

/* Si hay dos distintos entonces front!=back */
START_TEST (test_front_is_not_back)
{
	scommand_push_back(scmd, bfromcstr ("123"));
	scommand_push_back(scmd, bfromcstr ("456"));
	fail_unless (biseqcstr (scommand_front (scmd), "456") != 1, NULL);
}
END_TEST

/* Que la tupla de redirectores sea un par independiente */
START_TEST (test_redir)
{
	scommand_set_redir_in (scmd, bfromcstr ("123"));
	scommand_set_redir_out (scmd, bfromcstr ("456"));
	/* Los redirectores tienen que ser distintos */
	fail_unless (biseq (scommand_get_redir_in (scmd),
			scommand_get_redir_out (scmd)) != 1, NULL);
	/* ahora si ambos idem */
	scommand_set_redir_out (scmd, bfromcstr ("123"));
	fail_unless (biseq (scommand_get_redir_in (scmd), scommand_get_redir_out (scmd)) == 1, NULL);
}
END_TEST

START_TEST (test_independent_redirs)
{
	/* Cuando la gente copia y pega entre las funciones de redir_in y 
	 * redir_out, tiende a olvidarse de hacer s/in/out/. Probamos algunos
	 * casos simples
	 */

	/* Primero: sólo entrada */
	scommand_set_redir_in (scmd, bfromcstr ("123"));
	fail_unless (biseqcstr (scommand_get_redir_in (scmd), "123") == 1, NULL);
	fail_unless (scommand_get_redir_out (scmd) == NULL, NULL);

	/* Segundo: Volvemos ambos a NULL */
	scommand_set_redir_in (scmd, NULL);
	fail_unless (scommand_get_redir_in (scmd) == NULL, NULL);
	fail_unless (scommand_get_redir_out (scmd) == NULL, NULL);

	/* Tercero: sólo salida */
	scommand_set_redir_out (scmd, bfromcstr ("456"));
	fail_unless (scommand_get_redir_in (scmd) == NULL, NULL);
	fail_unless (biseqcstr (scommand_get_redir_out (scmd), "456") == 1, NULL);

	/* Cuarto: ambos */
	scommand_set_redir_in (scmd, bfromcstr ("123"));
	fail_unless (biseqcstr (scommand_get_redir_in (scmd), "123") == 1, NULL);
	fail_unless (biseqcstr (scommand_get_redir_out (scmd), "456") == 1, NULL);
}
END_TEST


/* Comando nuevo, string vacío */
START_TEST (test_to_string_empty)
{
	bstring str = NULL;
	str = scommand_to_string (scmd);
	fail_unless (blength (str) == 0, NULL);
	bdestroy (str);
}
END_TEST

/* Algo más fuerte. Poner muchos argumentos, mirar el orden 
 * Poner redirectores, mirar el orden
 */
START_TEST (test_to_string)
{
	bstring str = NULL;
	bstring *strings = NULL;
	int i = 0;
	int last_pos = 0;
	strings = calloc (MAX_LENGTH, sizeof(bstring));
	for (i=0; i<MAX_LENGTH; i++) {
		strings[i] = bformat ("%d", i);
	}
	/* strings = ["0","1", ..., "127"] */

	assert (MAX_LENGTH>2);
	/* comando "0 1 2 .... N-3 < N-2 > N-1" tiene que tener todos los números y los dos piquitos */
	for (i=0; i<MAX_LENGTH; i++) {
		if (i<MAX_LENGTH-2) {
			scommand_push_back (scmd, strings[i]);
		} else if (i==MAX_LENGTH-2) {
			scommand_set_redir_in (scmd, strings[i]);
		} else {
			assert(i==MAX_LENGTH-1);
			scommand_set_redir_out (scmd, strings[i]);			
		}
	}
	str = scommand_to_string (scmd);
	last_pos = 0;
	for (i=0; i<MAX_LENGTH; i++) {
		if (i<MAX_LENGTH-2) {
			fail_unless (binstr(str,0,strings[i])>=last_pos, NULL);
			last_pos = binstr (str,0,strings[i]);
		} else if (i==MAX_LENGTH-2) {
			fail_unless (binstr(str,0,strings[i])>=0, NULL);
			fail_unless (bstrchr(str, '<'), NULL);
			fail_unless (binstr(str,0,strings[i])>bstrchr(str, '<'), NULL);
		} else {
			assert(i==MAX_LENGTH-1);
			fail_unless (binstr(str,0,strings[i])>=0, NULL);
			fail_unless (bstrchr(str, '>'), NULL);
			fail_unless (binstr(str,0,strings[i])>bstrchr(str, '>'), NULL);
		}
	}
	bdestroy (str);
	free(strings);
}
END_TEST


/* Armado de la test suite */

Suite *scommand_suite (void)
{
	Suite *s = suite_create ("scommand");
	TCase *tc_preconditions = tcase_create ("Precondition");
	TCase *tc_creation = tcase_create ("Creation");
	TCase *tc_functionality = tcase_create ("Functionality");

	/* Precondiciones */
	tcase_add_test_raise_signal (tc_preconditions, test_destroy_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_push_back_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_push_back_argument_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_pop_front_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_pop_front_empty, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_set_redir_in_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_set_redir_out_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_is_empty_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_length_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_front_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_front_empty, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_get_redir_in_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_get_redir_out_null, SIGABRT);
	tcase_add_test_raise_signal (tc_preconditions, test_to_string_null, SIGABRT);
	suite_add_tcase (s, tc_preconditions);

	/* Creation */
	tcase_add_test (tc_creation, test_new_destroy);
	tcase_add_test (tc_creation, test_new_is_empty);
	suite_add_tcase (s, tc_creation);

	/* Funcionalidad */
	tcase_add_checked_fixture (tc_functionality, setup, teardown);
	tcase_add_test (tc_functionality, test_adding_emptying);
	tcase_add_test (tc_functionality, test_adding_emptying_length);
	tcase_add_test (tc_functionality, test_fifo);
	tcase_add_test (tc_functionality, test_front_idempotent);
	tcase_add_test (tc_functionality, test_front_is_back);
	tcase_add_test (tc_functionality, test_front_is_not_back);
	tcase_add_test (tc_functionality, test_redir);
	tcase_add_test (tc_functionality, test_independent_redirs);
	tcase_add_test (tc_functionality, test_to_string_empty);
	tcase_add_test (tc_functionality, test_to_string);
	suite_add_tcase (s, tc_functionality);

	return s;
}

/* Para testing de memoria */
void scommand_memory_test (void) {
	/* Las siguientes operaciones deberían poder hacer sin leaks ni doble 
	 * frees.
	 */

	/* Crear y destruir un scommand vacío */
	scmd = scommand_new ();
	scommand_destroy (scmd);

	/* Crear un scommand, llenarlo de argumentos, liberarlo
	 * sin liberar los argumentos desde afuera
	 */
	scmd = scommand_new ();
	scommand_push_back (scmd, bfromcstr ("un-argumento"));
	scommand_push_back (scmd, bfromcstr ("otro-argumento"));
	scommand_destroy(scmd);

	/* Crear un scommand, setearle redirectores, liberarlo
	 * sin liberar los redirectores desde afuera
	 */
	scmd = scommand_new ();
	scommand_set_redir_in (scmd, bfromcstr ("entrada"));
	scommand_set_redir_out (scmd, bfromcstr ("salida"));
	scommand_destroy(scmd);

	/* Meter, sacar, meter argumentos alternadamente */
	scmd = scommand_new ();
	scommand_push_back (scmd, bfromcstr ("uno"));
	scommand_push_back (scmd, bfromcstr ("dos"));
	scommand_push_back (scmd, bfromcstr ("cinco"));
	scommand_pop_front (scmd);
	scommand_push_back (scmd, bfromcstr ("perdon, tres"));
	scommand_push_back (scmd, bfromcstr ("cuatro"));
	scommand_destroy(scmd);

	/* Modificar redictores ya seteados */
	scmd = scommand_new ();
	scommand_set_redir_out (scmd, bfromcstr ("entrada"));
	scommand_set_redir_out (scmd, bfromcstr ("quise decir salida"));
	scommand_destroy(scmd);

	/* Usar todos los accesores */
	scmd = scommand_new ();
	scommand_push_back (scmd, bfromcstr ("comando"));
	scommand_push_back (scmd, bfromcstr ("un-argumento"));
	scommand_push_back (scmd, bfromcstr ("otro-argumento"));
	scommand_set_redir_out (scmd, bfromcstr ("salida"));
	scommand_is_empty (scmd);
	scommand_length (scmd);
	scommand_front (scmd);
	scommand_get_redir_in (scmd);
	scommand_get_redir_out (scmd);
	scommand_destroy (scmd);
	
	/* Se puede modificar el TAD luego de acceder a front, aún cuando eso
	 * invalida el resultado anterior de front. Sin leaks, al igual que todos
	 * estos casos.
	 */
	scmd = scommand_new ();
	scommand_push_back (scmd, bfromcstr ("comando"));
	scommand_front (scmd);
	scommand_pop_front (scmd);
	/* En este punto, s puede ser no válido. */
	scommand_destroy (scmd);
	 
	/* Al string que me devuelve to_string lo puedo liberar. Y por dentro, 
	 * no hay leaks, al igual que todos estos casos.
	 */
	scmd = scommand_new ();
	scommand_push_back (scmd, bfromcstr ("comando"));
	scommand_push_back (scmd, bfromcstr ("argument"));
	scommand_set_redir_out (scmd, bfromcstr ("salida"));
	bdestroy (scommand_to_string (scmd));
	scommand_destroy (scmd);
}

