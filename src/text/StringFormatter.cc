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
//StringFormatter.cc

#include "StringFormatter.h"
#include "boost/format.hpp"
#include <iostream>
#include <cstdio>

char StringFormatter::buf[StringFormatter::sz];

//! @brief Constructor por defecto.
StringFormatter::StringFormatter(const std::string &f)
  : fmt(f) {}

//! @brief Devuelve el número de cadenas de formato simples, presentes en fmt.
size_t StringFormatter::num_format_strings(void) const
  {
    size_t retval= 0;
    const size_t sz= fmt.size();
    for(size_t i= 0;i<sz;i++)
      {
        if(fmt[i] == '%')
          {
            retval++;
            if(i<sz)
              if(fmt[i+1]== '%') //"%%"
                i++; //Ignora el segundo '%'.
          }
      }
    return retval;
  }

//! @brief Devuelve el carácter especificador (%s->s,%4.2f->f,...) presente en fmt.
char StringFormatter::char_espec_format_string(void) const
  {
    char retval= '\0';
    const size_t fmtPos= fmt.find("%",0);
    //Buscamos el caracter especificador de formato.
    static const std::string CharSpec= "cCdiouxXeEfgGnpsS";
    const size_t charPos= fmt.find_first_of(CharSpec,fmtPos+1);
    if(charPos != std::string::npos) //Lo encontró.
      retval= fmt[charPos];
    return retval;
  }

//! @brief Devuelve el carácter especificador (%s->s,%4.2f->f,...) presente en fmt.
bool StringFormatter::check_format_string(const std::string &espec_prohibidos,const std::string &nmb_tipo) const
  {
    bool retval= true;
    const size_t nf= num_format_strings();
    if(nf!=1)
      {
        std::cerr << "Error en la cadena de formato: '" << fmt
                  << "' se esperaba un sólo especificador y se obtuvieron: "
                  << nf << ".\n";
        retval= false;
      }
    else
      {
        const char c_espec= char_espec_format_string();
        if(espec_prohibidos.find(c_espec)!=std::string::npos)
          {
	    std::cerr << "Error en la cadena de formato: '" << fmt
                      << "' el especificador: '" << c_espec
                      << "' está prohibido para variables de tipo: '"
                      << nmb_tipo << "'." << std::endl;
            retval= false;
          }
      }
    return retval;
  }

std::string StringFormatter::format(const std::string &str)
  {
    return boost::str(boost::format(fmt) % str);//int err= snprintf(buf,sz,fmt.c_str(),str.c_str());
  }

std::string StringFormatter::format(const bool &b)
  {
    std::string retval;
    if(check_format_string("s","bool"))
      retval= boost::str(boost::format(fmt) % b); //err= snprintf(buf,sz,fmt.c_str(),b);
    return retval;
  }

std::string StringFormatter::format(const int &i)
  {
    std::string retval;
    if(check_format_string("s","int"))
      retval= boost::str(boost::format(fmt) % i); //err= snprintf(buf,sz,fmt.c_str(),i);
    return retval;
  }

std::string StringFormatter::format(const long int &i)
  {
    std::string retval;
    if(check_format_string("s","long int"))
      retval= boost::str(boost::format(fmt) % i); //err= snprintf(buf,sz,fmt.c_str(),i);
    return retval;
  }

std::string StringFormatter::format(const size_t &s)
  {
    std::string retval;
    if(check_format_string("s","size_t"))
      retval= boost::str(boost::format(fmt) % s); //err= snprintf(buf,sz,fmt.c_str(),s);
    return retval;
  }

std::string StringFormatter::format(const double &d)
  {
    std::string retval;
    if(check_format_string("s","double"))
      retval= boost::str(boost::format(fmt) % d); //snprintf(buf,sz,fmt.c_str(),d);
    return retval;
  }

std::string StringFormatter::format(const float &f)
  {
    std::string retval;
    if(check_format_string("s","float"))
      retval= boost::str(boost::format(fmt) % f); // err= snprintf(buf,sz,fmt.c_str(),f);
    return retval;
  }

StringFormatter formatter("");

std::string format(const std::string &fmt,const std::string &str)
  {
    formatter.format_string()= fmt;
    return formatter.format(str);
  }

std::string format(const std::string &fmt,const bool &i)
  {
    formatter.format_string()= fmt;
    return formatter.format(i);
  }
std::string format(const std::string &fmt,const int &i)
  {
    formatter.format_string()= fmt;
    return formatter.format(i);
  }
std::string format(const std::string &fmt,const long int &i)
  {
    formatter.format_string()= fmt;
    return formatter.format(i);
  }
std::string format(const std::string &fmt,const size_t &i)
  {
    formatter.format_string()= fmt;
    return formatter.format(i);
  }
std::string format(const std::string &fmt,const double &d)
  {
    formatter.format_string()= fmt;
    return formatter.format(d);
  }
std::string format(const std::string &fmt,const float &f)
  {
    formatter.format_string()= fmt;
    return formatter.format(f);
  }
