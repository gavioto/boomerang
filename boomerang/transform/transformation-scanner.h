#ifndef FLEX_HEADER_transformation_scanner_h
#define FLEX_HEADER_transformation_scanner_h
#define YY_TransformationScanner_CHAR char
#line 1 "/home/50/trent/flex++-2.3.8-7/./flexskel.h"
/* A lexical scanner header generated by flex */
/* MODIFIED FOR C++ CLASS BY Alain Coetmeur: coetmeur(at)icdc.fr */
/* Note that (at) mean the 'at' symbol that I cannot write */
/* because it is expanded to the class name */
/* made at Informatique-CDC, Research&development department */
/* company from the Caisse Des Depots et Consignations */


/*********************************************/
/* SYSTEM dependent declaration, includes... */
/*********************************************/
/* cfront 1.2 defines "c_plusplus" instead of "__cplusplus" */
#ifdef c_plusplus
#ifndef __cplusplus
#define __cplusplus
#endif
#endif
#ifdef __cplusplus
#ifndef YY_USE_PROTOS
#define YY_USE_PROTOS
#endif
#ifndef YY_USE_CLASS
#define YY_USE_CLASS
#endif
#else	/* ! __cplusplus */
#ifdef __STDC__
#ifdef __GNUC__
#else
#endif	/* __GNUC__ */
#ifndef YY_USE_PROTOS
#define YY_USE_PROTOS
#endif
#endif	/* __STDC__ */
#endif	/* ! __cplusplus */
/*********************************************/
/* COMPILER DEPENDENT   MACROS               */
/*********************************************/
/* use prototypes in function declarations */
#ifndef YY_PROTO
#ifdef YY_USE_PROTOS
#define YY_PROTO(proto) proto
#else
#define YY_PROTO(proto) ()
#endif
#endif
#include <stdio.h>




/* % here is the declaration from section1 %header{  */ 
#line 21 "transformation.l"
#include <stdio.h>
#include "transformation-parser.h"
// For some reason, MSVC 5.00 complains about use of undefined type RTL a lot
#if defined(_MSC_VER) && _MSC_VER <= 1100
#include "signature.h"		// For MSVC 5.00
#include "rtl.h"
#endif
#define MAX_LINE 1024      // the longest line
#line 31 "transformation.l"
#define YY_TransformationScanner_CONSTRUCTOR_PARAM  std::istream &ins, bool trace
#line 32 "transformation.l"
#define YY_TransformationScanner_CONSTRUCTOR_INIT  : theLine(1), traceLines(trace), in(ins), column(0)
#line 33 "transformation.l"
#define YY_TransformationScanner_CONSTRUCTOR_CODE  { *lineBuf = 0; }
#line 34 "transformation.l"
#define YY_TransformationScanner_INPUT_CODE  if (in.eof()) {  \
    result = 0; \
} else { \
    in.read(buffer, max_size); \
    result=in.gcount(); \
} \
return result;
#line 41 "transformation.l"
#define YY_TransformationScanner_MEMBERS  \
public: \
    int theLine;        /* the current line number */\
    char lineBuf[MAX_LINE]; /* the current line */ \
    bool traceLines;    /* echo each lines as it is scanned */ \
    std::istream &in; \
    int column; \
private: \
    void count(); \
    void comment();
#line 52 "transformation.l"
#define YY_TransformationScanner_LEX_PARAM  YY_TransformationParser_STYPE &yylval
#line 54 "transformation.l"
#line 52 "/home/50/trent/flex++-2.3.8-7/./flexskel.h"

#ifdef YY_USE_CLASS
#ifdef YY_TransformationScanner_IOSTREAM
#include <iostream.h>
#define YY_TransformationScanner_IFILE  istream
#define YY_TransformationScanner_OFILE ostream 
#define YY_TransformationScanner_ERRFILE cerr

#ifndef YY_TransformationScanner_IFILE_DEFAULT
#define YY_TransformationScanner_IFILE_DEFAULT &cin
#endif

#ifndef YY_TransformationScanner_OFILE_DEFAULT
#define YY_TransformationScanner_OFILE_DEFAULT &cout
#endif

#endif
#endif

#ifndef YY_TransformationScanner_IFILE
#define YY_TransformationScanner_IFILE FILE 
#endif

#ifndef YY_TransformationScanner_OFILE
#define YY_TransformationScanner_OFILE FILE 
#endif

#ifndef YY_TransformationScanner_ERRFILE
#define YY_TransformationScanner_ERRFILE stderr
#endif

#ifndef YY_TransformationScanner_IFILE_DEFAULT
#define YY_TransformationScanner_IFILE_DEFAULT stdin
#endif

#ifndef YY_TransformationScanner_OFILE_DEFAULT
#define YY_TransformationScanner_OFILE_DEFAULT stdout
#endif




#ifndef YY_TransformationScanner_TEXT
#define YY_TransformationScanner_TEXT yytext
#endif
#ifndef YY_TransformationScanner_LENG
#define YY_TransformationScanner_LENG yyleng
#endif
#ifndef YY_TransformationScanner_IN
#define YY_TransformationScanner_IN yyin
#endif
#ifndef YY_TransformationScanner_OUT
#define YY_TransformationScanner_OUT yyout
#endif

#ifndef YY_TransformationScanner_LEX_RETURN
#define YY_TransformationScanner_LEX_RETURN int
#else
#ifndef YY_TransformationScanner_LEX_DEFINED
#define YY_TransformationScanner_LEX_DEFINED 
#endif
#endif

#ifndef YY_TransformationScanner_LEX
#define YY_TransformationScanner_LEX yylex
#else
#ifndef YY_TransformationScanner_LEX_DEFINED
#define YY_TransformationScanner_LEX_DEFINED 
#endif
#endif

#ifndef YY_TransformationScanner_LEX_PARAM
#ifndef YY_USE_PROTOS
#define YY_TransformationScanner_LEX_PARAM 
#else
#define YY_TransformationScanner_LEX_PARAM void
#endif
#else
#ifndef YY_TransformationScanner_LEX_DEFINED
#define YY_TransformationScanner_LEX_DEFINED 
#endif
#endif

#ifndef YY_TransformationScanner_LEX_PARAM_DEF
#define YY_TransformationScanner_LEX_PARAM_DEF
#else
#ifndef YY_TransformationScanner_LEX_DEFINED
#define YY_TransformationScanner_LEX_DEFINED 
#endif
#endif

#ifndef YY_TransformationScanner_RESTART
#define YY_TransformationScanner_RESTART yyrestart
#endif
#ifndef YY_TransformationScanner_SWITCH_TO_BUFFER
#define YY_TransformationScanner_SWITCH_TO_BUFFER yy_switch_to_buffer
#endif
#ifndef YY_TransformationScanner_LOAD_BUFFER_STATE
#define YY_TransformationScanner_LOAD_BUFFER_STATE yy_load_buffer_state
#endif

#ifndef YY_TransformationScanner_CREATE_BUFFER
#define YY_TransformationScanner_CREATE_BUFFER yy_create_buffer
#ifndef YY_USE_CLASS
#ifndef yy_new_buffer
#define yy_new_buffer yy_create_buffer
#endif
#endif
#endif
#ifndef YY_TransformationScanner_DELETE_BUFFER
#define YY_TransformationScanner_DELETE_BUFFER yy_delete_buffer
#endif
#ifndef YY_TransformationScanner_INIT_BUFFER
#define YY_TransformationScanner_INIT_BUFFER yy_init_buffer
#endif



#ifdef YY_TransformationScanner_FLEX_DEBUG
#ifndef YY_TransformationScanner_DEBUG
#define YY_TransformationScanner_DEBUG 1
#endif
#else
#ifndef YY_TransformationScanner_DEBUG
#define YY_TransformationScanner_DEBUG 0
#endif
#endif

#if YY_TransformationScanner_DEBUG != 0
#ifndef YY_TransformationScanner_DEBUG_FLAG
#define YY_TransformationScanner_DEBUG_FLAG yy_flex_debug
#endif
#ifndef YY_TransformationScanner_DEBUG_INIT
#define YY_TransformationScanner_DEBUG_INIT 1
#endif
#endif




#ifndef YY_USE_CLASS
#ifndef YY_TransformationScanner_CURRENT_BUFFER
#define YY_TransformationScanner_CURRENT_BUFFER yy_current_buffer
#endif
typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern void YY_TransformationScanner_RESTART YY_PROTO(( YY_TransformationScanner_IFILE *input_file ));
extern void YY_TransformationScanner_SWITCH_TO_BUFFER YY_PROTO(( YY_BUFFER_STATE new_buffer ));
extern void YY_TransformationScanner_LOAD_BUFFER_STATE YY_PROTO(( void ));
extern YY_BUFFER_STATE YY_TransformationScanner_CREATE_BUFFER YY_PROTO(( YY_TransformationScanner_IFILE *file, int size ));
extern void YY_TransformationScanner_DELETE_BUFFER YY_PROTO(( YY_BUFFER_STATE b ));
extern void YY_TransformationScanner_INIT_BUFFER YY_PROTO(( YY_BUFFER_STATE b, YY_TransformationScanner_IFILE *file ));

#if YY_TransformationScanner_DEBUG != 0
extern int YY_TransformationScanner_DEBUG_FLAG ;
#endif
extern YY_TransformationScanner_CHAR  *YY_TransformationScanner_TEXT;
extern int YY_TransformationScanner_LENG;
extern YY_TransformationScanner_IFILE *YY_TransformationScanner_IN;
extern YY_TransformationScanner_OFILE *YY_TransformationScanner_OUT;
#ifdef YY_TransformationScanner_LEX_DEFINED
extern YY_TransformationScanner_LEX_RETURN YY_TransformationScanner_LEX ( YY_TransformationScanner_LEX_PARAM )
YY_TransformationScanner_LEX_PARAM_DEF
#else
#ifndef YY_DECL
extern YY_TransformationScanner_LEX_RETURN YY_TransformationScanner_LEX ( YY_TransformationScanner_LEX_PARAM )
YY_TransformationScanner_LEX_PARAM_DEF
#else
/* no declaration if oldstyle flex */
#endif
#endif
#else

#ifndef YY_TransformationScanner_CURRENT_BUFFER
#define YY_TransformationScanner_CURRENT_BUFFER YY_CURRENT_BUFFER
#endif
#ifndef YY_TransformationScanner_CLASS
#define YY_TransformationScanner_CLASS TransformationScanner
#endif
#ifndef YY_TransformationScanner_ECHO
#define YY_TransformationScanner_ECHO yy_echo
#endif
#ifdef YY_TransformationScanner_ECHO_PURE
#define YY_TransformationScanner_ECHO_NOCODE
#endif

#ifndef YY_TransformationScanner_ECHO_CODE
#ifndef YY_TransformationScanner_IOSTREAM
#define YY_TransformationScanner_ECHO_CODE fwrite( (char *) YY_TransformationScanner_TEXT, YY_TransformationScanner_LENG, 1, YY_TransformationScanner_OUT );
#else
#define YY_TransformationScanner_ECHO_CODE (YY_TransformationScanner_OUT->write( (char *) YY_TransformationScanner_TEXT, YY_TransformationScanner_LENG));
#endif
#endif

#ifndef YY_TransformationScanner_INPUT
#define YY_TransformationScanner_INPUT yy_input
#endif
#ifdef YY_TransformationScanner_INPUT_PURE
#define YY_TransformationScanner_INPUT_NOCODE
#endif

#ifndef YY_TransformationScanner_INPUT_CODE
#ifndef YY_TransformationScanner_IOSTREAM
#define YY_TransformationScanner_INPUT_CODE return result= fread(  buffer, 1,max_size,YY_TransformationScanner_IN );
#else
#define YY_TransformationScanner_INPUT_CODE if(YY_TransformationScanner_IN->eof())  result=0;else {YY_TransformationScanner_IN->read(buffer,max_size);result=YY_TransformationScanner_IN->gcount();YY_TransformationScanner_IN->clear(YY_TransformationScanner_IN->rdstate()&(~ios::failbit));if(YY_TransformationScanner_IN->bad()) result= -1;} return result;
#endif
#endif

#ifdef YY_TransformationScanner_FATAL_ERROR_PURE
#define YY_TransformationScanner_FATAL_ERRO_NOCODE
#endif
#ifndef YY_TransformationScanner_FATAL_ERROR
#define YY_TransformationScanner_FATAL_ERROR yy_fatal_error
#endif

#ifndef YY_TransformationScanner_FATAL_ERROR_CODE
#ifndef YY_TransformationScanner_IOSTREAM
#define YY_TransformationScanner_FATAL_ERROR_CODE fputs( msg, YY_TransformationScanner_ERRFILE );putc( '\n', YY_TransformationScanner_ERRFILE );exit( 1 );
#else
#define YY_TransformationScanner_FATAL_ERROR_CODE YY_TransformationScanner_ERRFILE<< msg <<endl;exit( 1 );
#endif
#endif

#ifndef YY_TransformationScanner_WRAP
#define YY_TransformationScanner_WRAP yy_wrap
#endif
#ifdef YY_TransformationScanner_WRAP_PURE
#define YY_TransformationScanner_WRAP_NOCODE
#endif
#ifndef YY_TransformationScanner_WRAP_CODE
#define YY_TransformationScanner_WRAP_CODE return 1;
#endif


#ifndef YY_TransformationScanner_INHERIT
#define YY_TransformationScanner_INHERIT
#endif
#ifndef YY_TransformationScanner_MEMBERS
#define YY_TransformationScanner_MEMBERS 
#endif
#ifndef YY_TransformationScanner_CONSTRUCTOR_PARAM
#define YY_TransformationScanner_CONSTRUCTOR_PARAM
#endif
#ifndef YY_TransformationScanner_CONSTRUCTOR_CODE
#define YY_TransformationScanner_CONSTRUCTOR_CODE
#endif
#ifndef YY_TransformationScanner_CONSTRUCTOR_INIT
#define YY_TransformationScanner_CONSTRUCTOR_INIT
#endif
#ifndef YY_TransformationScanner_DESTRUCTOR_CODE
#define YY_TransformationScanner_DESTRUCTOR_CODE
#endif
typedef struct yy_buffer_state *YY_BUFFER_STATE;

class YY_TransformationScanner_CLASS YY_TransformationScanner_INHERIT
{
 private:/* data */
 YY_TransformationScanner_CHAR  *yy_c_buf_p;
 YY_TransformationScanner_CHAR  yy_hold_char;
 int yy_n_chars;
 int yy_init;
 int yy_start;
 int yy_did_buffer_switch_on_eof;
 private: /* functions */
 void yy_initialize();
 int input();
 int yyinput() {return input();};
 int yy_get_next_buffer();
 void yyunput( YY_TransformationScanner_CHAR  c, YY_TransformationScanner_CHAR  *buf_ptr );
 /* use long instead of yy_state_type because it is undef */
 long yy_get_previous_state_ ( void );
 long yy_try_NUL_trans_  ( long current_state_ );
 protected:/* non virtual */
 YY_BUFFER_STATE YY_TransformationScanner_CURRENT_BUFFER;
 void YY_TransformationScanner_RESTART ( YY_TransformationScanner_IFILE *input_file );
 void YY_TransformationScanner_SWITCH_TO_BUFFER( YY_BUFFER_STATE new_buffer );
 void YY_TransformationScanner_LOAD_BUFFER_STATE( void );
 YY_BUFFER_STATE YY_TransformationScanner_CREATE_BUFFER( YY_TransformationScanner_IFILE *file, int size );
 void YY_TransformationScanner_DELETE_BUFFER( YY_BUFFER_STATE b );
 void YY_TransformationScanner_INIT_BUFFER( YY_BUFFER_STATE b, YY_TransformationScanner_IFILE *file );
 protected: /* virtual */
 virtual void YY_TransformationScanner_ECHO()
#ifdef YY_TransformationScanner_ECHO_PURE
  =0
#endif
  ;
 virtual int  YY_TransformationScanner_INPUT(char  *buf,int &result,int max_size)
#ifdef YY_TransformationScanner_INPUT_PURE
  =0
#endif
  ;
 virtual void  YY_TransformationScanner_FATAL_ERROR(char *msg)
#ifdef YY_TransformationScanner_FATAL_ERROR_PURE
  =0
#endif
  ;
 virtual int  YY_TransformationScanner_WRAP()
#ifdef YY_TransformationScanner_WRAP_PURE
  =0
#endif
  ;
 public:
 YY_TransformationScanner_CHAR  *YY_TransformationScanner_TEXT;
 int YY_TransformationScanner_LENG;
 YY_TransformationScanner_IFILE *YY_TransformationScanner_IN;
 YY_TransformationScanner_OFILE *YY_TransformationScanner_OUT;
 YY_TransformationScanner_LEX_RETURN YY_TransformationScanner_LEX ( YY_TransformationScanner_LEX_PARAM);
 YY_TransformationScanner_CLASS(YY_TransformationScanner_CONSTRUCTOR_PARAM) ;
 virtual ~YY_TransformationScanner_CLASS() ;
#if YY_TransformationScanner_DEBUG != 0
 int YY_TransformationScanner_DEBUG_FLAG;
#endif
 public: /* added members */
 YY_TransformationScanner_MEMBERS 
};
#endif



/* declaration of externs for public use of yylex scanner */

/* % here is the declaration from section2 %header{ */ 
#line 136 "transformation.l"
#endif
#line 375 "/home/50/trent/flex++-2.3.8-7/./flexskel.h"

/* end of generated header */
