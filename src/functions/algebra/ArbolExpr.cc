//----------------------------------------------------------------------------
//  xc_basic library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ArbolExpr.cc
#include "Expresion.h"
#include "ArbolExpr.h"
#include "LexAlgebra.h"
#include "ConjVariables.h"
#include "ExprPostfija.h"
#include "ExprFunctor.h"
#include "xc_basic/src/text/text_string.h"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include "MapValores.h"

//! @brief Constructor por defecto.
ArbolExpr::ArbolExpr(void): ExprBase(false), raiz(NULL) {}

//! @brief Constructor a partir de text string.
ArbolExpr::ArbolExpr(const std::string &str)
  : ExprBase(false), raiz(NULL)
  { InicFromStr(str); }

//! @brief Constructor de copia.
ArbolExpr::ArbolExpr(const ArbolExpr &otro)
  : ExprBase(otro), raiz(NULL) 
  {
    if(otro.raiz)
      raiz= otro.raiz->getCopy();
  }

//! @brief Constructor a partir de double.
ArbolExpr::ArbolExpr(const double &d)
  : ExprBase(false), raiz(new Rama(d)) {}

ArbolExpr::~ArbolExpr(void)
  {
    if(raiz)
      {
        delete raiz;
        raiz= NULL;
      }
  }

Rama *ArbolExpr::Traduce(const std::string &cadena_entrada)
  {
    Rama *retval= NULL;
    ExprPostfija postfija(cadena_entrada);
    if(postfija.ErrorTraduccion())
      {
        std::cerr << "ArbolExpr::Traduce; se produjo un error al interpretar la expresión: '"
                  << cadena_entrada << "', se devuelve NULL." << std::endl;
        err_traduc= true;
      }
    else
      retval= Traduce(postfija);
    return retval;
  }

Rama *ArbolExpr::Traduce(const ExprPostfija &e)
  {
    if (e.Size() == 0) return NULL;
    Pila pila;
    register ExprPostfija::const_iterator_segnales i= e.Begin();
    const Operando *s= NULL;
    while(i != e.End())
      {
        s= dynamic_cast<const Operando *>(*i);
        i++;
        switch(s->GetClase())
          {
	  case operando:
            InsertaOperando(pila,s);
            break;
	  case op_unitario:
            InsertaOpUnario(pila,s);
            break;
          case op_binario:
            InsertaOpBinario(pila,s);
            break;
	  case parent_dcho:
          case parent_izdo:
	  case fin_exp:
            break;
          }
      }
    return FinExpresion(pila);
  }

void ArbolExpr::InicFromStr(const std::string &str)
  {
    if(raiz)
      {
        delete raiz;
        raiz= NULL;
      }
    if(isNumber(str)) //La cadena representa un número (no una expresión)
      raiz= new Rama(boost::lexical_cast<double>(str));
    else
      {
        raiz= Traduce(str);
        OperaToNum(); //Procesa los operadores ToNum;
      }
  }


void ArbolExpr::RecorrePreorden(const ExprFunctor &f)
  { if(raiz) ::RecorrePreorden(raiz,f); }
void ArbolExpr::RecorreEnorden(const ExprFunctor &f)
  { if(raiz) ::RecorreEnorden(raiz,f); }
void ArbolExpr::RecorrePostorden(const ExprFunctor &f)
  { if(raiz) ::RecorrePostorden(raiz,f); }
void ArbolExpr::RecorrePreorden(const ExprFunctor &f) const
  { if(raiz) ::RecorrePreorden(raiz,f); }
void ArbolExpr::RecorreEnorden(const ExprFunctor &f) const
  { if(raiz) ::RecorreEnorden(raiz,f); }
void ArbolExpr::RecorrePostorden(const ExprFunctor &f) const
  { if(raiz) ::RecorrePostorden(raiz,f); }


void ArbolExpr::Asigna(const std::string &palabra,const ArbolExpr &a)
  {
    Variable *v= CalcDirVar(palabra);
//     if(!v)
//       std::cerr << "¡Ojo! no se encontró la variable: " << palabra << std::endl;
    if(v && raiz)
      {
        if(raiz->GetData() == v) //Es la propia raiz.
          operator=(a);
        else
          {
            FctorAsigna fa(v,a.raiz);
            RecorrePostorden(fa);
          }
      }
  }

//! @brief Opera todo el árbol.
void ArbolExpr::Opera(void)
  {
    static FctorOpera f;
    RecorrePostorden(f);
  }

//! @brief Procesa los operadores to_num que haya en la expresión.
void ArbolExpr::OperaToNum(void)
  {
     static FctorOperaToNum ftn;
     RecorrePostorden(ftn);
  }

const double &ArbolExpr::ToNum(void) const
  //Devuelve un doble si puede.
  {
    ArbolExpr tmp(*this);
    tmp.Opera();
    const double &retval= tmp.GetValor();
    if(std::isnan(retval))
      std::cerr << "¡Ojo! No se pudo evaluar la expresión: " << *this << std::endl;
    return retval;
  }

//! @brief Devuelve el valor numérico (si puede).
const double &ArbolExpr::ToNum(const std::string &palabra,const double &d) const
  {
    Asigna(palabra,d);
    return ToNum();
  }

const double &ArbolExpr::ToNum(const MapValores &mv) const
  {
    ArbolExpr tmp(*this);
    tmp.Opera();
    for(MapValores::const_iterator i= mv.begin();i!=mv.end();i++)
      tmp.Asigna((*i).first,(*i).second);
    return tmp.ToNum();
  }

//! @brief Cambia de signo la expresión contenida en el árbol.
void ArbolExpr::Neg(void)
  {
    Rama *tmp= raiz;
    raiz= new Rama(&(ptr_lex->neg),NULL,tmp);
  }

//! @brief Aplica el operador "valor absoluto" a la expresión contenida en el árbol.
void ArbolExpr::Abs(void)
  {
    Rama *tmp= raiz;
    raiz= new Rama(&(ptr_lex->abs),NULL,tmp);
  }

//! @brief Aplica el operador "raíz cuadrada" a la expresión contenida en el árbol.
void ArbolExpr::Sqrt(void)
  {
    Rama *tmp= raiz;
    raiz= new Rama(&(ptr_lex->raiz2),NULL,tmp);
  }


//! @brief Diferencia la expresión respecto a la variable cuyo identificador se pasa como parámetro.
void ArbolExpr::Dif(const std::string &var)
  {
    if(raiz)
      {
        Variable *v= CalcDirVar(var);
        if(v)
          {
            Rama tmp= diferencia(*raiz,*v);
            delete raiz;
            raiz= tmp.getCopy();
          }
      }
  }

ArbolExpr &ArbolExpr::operator+=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->suma),a2); }
ArbolExpr &ArbolExpr::operator-=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->resta),a2); }
ArbolExpr &ArbolExpr::operator*=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->prodt),a2); }
ArbolExpr &ArbolExpr::operator/=(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->divn),a2); }
ArbolExpr &ArbolExpr::pow(const ArbolExpr &a2)
  { return AplicaOperador(&(ptr_lex->pot),a2); }

ArbolExpr &ArbolExpr::AplicaOperador(const Operador *op,const ArbolExpr &a2)
  {
    Rama *tmp= raiz;
    raiz= new Rama(op,tmp,a2.raiz->getCopy());
    return *this;
  }
//! @brief Devuelve el conjunto de variables que aparecen en la expresión.
ConjVariables ArbolExpr::Variables() const
  {
    ConjVariables cvar;
    if(raiz)
      {
        FctorGetVariables fa(cvar);
        RecorrePostorden(fa);
      }
    return cvar;
  }

//! @brief Devuelve verdadero si la expresión depende de la variable cuyo
//! nombre se pasa como parámetro.
bool ArbolExpr::TieneVariable(const std::string &palabra) const
  {
    const Variable *v= CalcDirVar(palabra);
    return TieneVariable(*v);
  }
//! @brief Devuelve verdadero si la expresión depende de la variable que
//! se pasa como parámetro.
bool ArbolExpr::TieneVariable(const VarExpr &var) const
  {
    const Variable *v= dynamic_cast<const Variable *>(&var);
    return TieneVariable(*v);
  }
//! @brief Devuelve verdadero si la expresión depende de la variable que
//! se pasa como parámetro.
bool ArbolExpr::TieneVariable(const Variable &var) const
  {
    ConjVariables vars= Variables();
    return vars.In(&var);
  }

//! @brief Devuelve el número de variables que intervienen en la expresión.
int ArbolExpr::NumVariables(void) const
  { return Variables().size(); }

std::vector<std::string> ArbolExpr::getNombresVariables(void) const
  { return Variables().getNombres(); }

//! @brief Imprime la expresión en el stream que se pasa como parámetro.
void ArbolExpr::Print(std::ostream &os) const
  { os << GetFullString(); }

//! @brief Devuelve una text string con el contenido de la expresión.
std::string ArbolExpr::GetString(void) const
  {
    std::ostringstream retval;
    Print(retval);
    return retval.str();
  }
//! @brief Devuelve una text string con el contenido de la expresión
//! representando los valores reales con toda la precisión posible.
const std::string &ArbolExpr::GetFullString(void) const
  {
    static std::string retval;
    if(raiz) retval= raiz->GetFullString();
    return retval;
  }

//! @brief Inserta en stream de salida.
std::ostream &operator<<(std::ostream &os,const ArbolExpr &a)
  {
    //a.Print(stream);
    os << a.GetFullString();
    return os;
  }

//! @brief Lee del stream de entrada una expresión entre comillas simples.
std::istream &operator>>(std::istream &is,ArbolExpr &a)
  {
    char c;
    unsigned short int conta= 0; //Cuenta las comillas simples.
    std::string tmp= "";
    while(!is.eof())
      {
        is >> c;
        if(c == '\'')
          {
            conta++;
            if(conta>1) break;
          }
        else
          if(conta==1)
            tmp+= c;
      }
    if(conta<2)
      std::cerr << "Error en lectura de expresión, faltan comillas simples." << std::endl;
    a= ArbolExpr(tmp);
    return is;
  }

//! @brief Devuelve el valor absoluto del argumento.
ArbolExpr abs(const ArbolExpr &otro)
  {
    ArbolExpr retval(otro);
    retval.Abs();
    return retval;
  }

//! @brief Devuelve la raíz cuadrada del argumento.
ArbolExpr sqrt(const ArbolExpr &otro)
  {
    ArbolExpr retval(otro);
    retval.Sqrt();
    return retval;
  }
