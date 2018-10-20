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
//PolynomialMatrix.cc

#include "PolynomialMatrix.h"
#include "m_double.h"

PolynomialMatrix::PolynomialMatrix(const m_double &m)
  : polynomial_matrix(m.getNumberOfRows(),m.getNumberOfColumns())
  {
    for(register size_type i=1;i<=n_rows;i++)
      for(register size_type j=1;j<=n_columns;j++)
        (*this)(i,j)= Polinomio(m(i,j));
  }
void PolynomialMatrix::eval(short unsigned int k,const double &val)
  {
    for(register size_type i=1;i<=n_rows;i++)
      for(register size_type j=1;j<=n_columns;j++)
        (*this)(i,j)= (*this)(i,j).Eval(k,val);
  }
void PolynomialMatrix::eval(short unsigned int k,const Polinomio &val)
  {
    for(register size_type i=1;i<=n_rows;i++)
      for(register size_type j=1;j<=n_columns;j++)
        (*this)(i,j)= (*this)(i,j).Eval(k,val);
  }
void PolynomialMatrix::eval(const vZ_double &v)
  {
    for(register size_type i=1;i<=n_rows;i++)
      for(register size_type j=1;j<=n_columns;j++)
        (*this)(i,j)= (*this)(i,j).Eval(v);
  }
void PolynomialMatrix::eval(const mZ_double &v)
  {
    for(register size_type i=1;i<=n_rows;i++)
      for(register size_type j=1;j<=n_columns;j++)
        (*this)(i,j)= (*this)(i,j).Eval(v);
  }
PolynomialMatrix PolynomialMatrix::Eval(short unsigned int j,const double &val) const
  {
    PolynomialMatrix retval(*this);
    retval.eval(j,val);
    return retval;
  }
PolynomialMatrix PolynomialMatrix::Eval(short unsigned int j,const Polinomio &val) const
  {
    PolynomialMatrix retval(*this);
    retval.eval(j,val);
    return retval;
  }
PolynomialMatrix PolynomialMatrix::Eval(const vZ_double &v) const
  {
    PolynomialMatrix retval(*this);
    retval.eval(v);
    return retval;
  }
PolynomialMatrix PolynomialMatrix::Eval(const mZ_double &v) const
  {
    PolynomialMatrix retval(*this);
    retval.eval(v);
    return retval;
  }
PolynomialMatrix &PolynomialMatrix::Trn(void)
  {
    polynomial_matrix::Trn();
    return *this;
  }
PolynomialMatrix PolynomialMatrix::GetTrn(void) const
  {
    PolynomialMatrix m= *this;
    m.Trn();
    return m;
  }
PolynomialMatrix &PolynomialMatrix::operator*=(const double &d)
  {
    const size_type sz= size();      
    for(register size_type i= 0;i<sz;i++)
      (*this)[i]*=d;
    return *this;
  }
PolynomialMatrix &PolynomialMatrix::operator+=(const m_double &m)
  {
    if (!CompDim(*this,m))
      {
        std::cerr << "Matrices de dimensiones distintas en operador +=" << std::endl;
        abort();
      }
    PolynomialMatrix::size_type n_rows= getNumberOfRows(), n_columns= getNumberOfColumns();
    register PolynomialMatrix::size_type i,j;
    for (i=1;i<=n_rows;i++)
      for (j=1;j<=n_columns;j++)
        (*this)(i,j)+= m(i,j);
    return *this;
  }
PolynomialMatrix &PolynomialMatrix::operator-=(const m_double &m)
  {
    if (!CompDim(*this,m))
      {
        std::cerr << "Matrices de dimensiones distintas en operador +=" << std::endl;
        abort();
      }
    PolynomialMatrix::size_type n_rows= getNumberOfRows(), n_columns= getNumberOfColumns();
    register PolynomialMatrix::size_type i,j;
    for (i=1;i<=n_rows;i++)
      for (j=1;j<=n_columns;j++)
        (*this)(i,j)-= m(i,j);
    return *this;
  }

PolynomialMatrix &PolynomialMatrix::Primitiva(short unsigned int k)
  {
    const PolynomialMatrix::size_type n_rows= getNumberOfRows();
    const PolynomialMatrix::size_type n_columns= getNumberOfColumns();
    PolynomialMatrix::size_type i,j;
    for (i=1;i<=n_rows;i++)
      for (j=1;j<=n_columns;j++)
        (*this)(i,j)= (*this)(i,j).Primitiva(k);
    return *this;
  }

PolynomialMatrix &PolynomialMatrix::Diferencial(short unsigned int k)
  {
    const PolynomialMatrix::size_type n_rows= getNumberOfRows();
    const PolynomialMatrix::size_type n_columns= getNumberOfColumns();
    PolynomialMatrix::size_type i,j;
    for (i=1;i<=n_rows;i++)
      for (j=1;j<=n_columns;j++)
        (*this)(i,j)= (*this)(i,j).Parcial(k);
    return *this;
  }

PolynomialMatrix operator*(const PolynomialMatrix &m1,const PolynomialMatrix &m2)
  {
    PolynomialMatrix producto(m1.n_rows,m2.n_columns);
    if (m1.n_columns != m2.n_rows)
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto." << std::endl;
        std::cerr << "  m1= " << m1 << std::endl;
        std::cerr << "  m2= " << m2 << std::endl;
        return producto;
      }
    PolynomialMatrix::size_type i=1,j=1;
    for (i= 1;i<=m1.n_rows;i++)
      for (j= 1;j<=m2.n_columns;j++)
        producto(i,j) = dot(m1.getRow(i),m2.getColumn(j));
    return producto;
  }
PolynomialMatrix operator+(const PolynomialMatrix &m1,const PolynomialMatrix &m2)
  {
    PolynomialMatrix retval(m1);
    retval+=(m2);
    return retval;
  }
PolynomialMatrix operator-(const PolynomialMatrix &m1,const PolynomialMatrix &m2)
  {
    PolynomialMatrix retval(m1);
    retval+=(m2);
    return retval;
  }
PolynomialMatrix operator+(const PolynomialMatrix &m1,const m_double &m2)
  {
    PolynomialMatrix retval(m1);
    retval+=(m2);
    return retval;
  }
PolynomialMatrix operator-(const PolynomialMatrix &m1,const m_double &m2)
  {
    PolynomialMatrix retval(m1);
    retval-=(m2);
    return retval;
  }
PolynomialMatrix operator-(const m_double &m1,const PolynomialMatrix &m2)
  { return PolynomialMatrix(m1)-m2; }

PolynomialMatrix operator-(const PolynomialMatrix &m)
  {
    PolynomialMatrix retval(m);
    retval.Neg();
    return retval;
  }

//Return el valor del polinomio en el punto vp.
Polinomio Eval(const Polinomio &p,const PolynomialMatrix &vp)
  {
    Polinomio result= p;
    size_t i,sz= vp.getNumberOfRows();
    for (i=1;i<=sz;i++) result= result.Eval(i,vp(i,1));
    return result;
  }

m_double Eval(const PolynomialMatrix &p,const m_double &x)
  {
    PolynomialMatrix::size_type n_rows= p.getNumberOfRows();
    PolynomialMatrix::size_type n_columns= p.getNumberOfColumns();
    m_double f(n_rows,n_columns);
    PolynomialMatrix::size_type i,j;
    for (i=1;i<=n_rows;i++)
      for (j=1;j<=n_columns;j++)
        f(i,j)= p(i,j).Eval(x);
    return f;
  }

PolynomialMatrix Eval(const PolynomialMatrix &m1,const PolynomialMatrix &m2)
  {
    PolynomialMatrix::size_type n_rows= m1.getNumberOfRows();
    PolynomialMatrix::size_type n_columns= m1.getNumberOfColumns();
    PolynomialMatrix f(n_rows,n_columns);
    PolynomialMatrix::size_type i,j;
    for (i=1;i<=n_rows;i++)
      for (j=1;j<=n_columns;j++)
        f(i,j)= Eval(m1(i,j),m2);
    return f;
  }
Polinomio dot(const PolynomialMatrix &v1,const m_double &v2)
//Producto escalar de dos vectores.
//v1: row vector.
//v2: column vector.
  {
    const PolynomialMatrix::size_type n_columns= v1.getNumberOfColumns();
    if (n_columns != v2.getNumberOfRows())
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto escalar." << std::endl;
        abort();      
      }
    Polinomio retval= Polinomio::neutro_suma();
    for(register PolynomialMatrix::size_type i=1;i<=n_columns;i++)
      retval+= v1(1,i) * v2(i,1);
    return retval;
  }
Polinomio dot(const m_double &v1,const PolynomialMatrix &v2)
//Producto escalar de dos vectores.
//v1: row vector.
//v2: column vector.
  {
    const m_double::size_type n_columns= v1.getNumberOfColumns();
    if (n_columns != v2.getNumberOfRows())
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto escalar." << std::endl;
        std::cerr << "  v1= " << v1 << std::endl;
        std::cerr << "  v2= " << v2 << std::endl;
        abort();
      }
    Polinomio retval= Polinomio::neutro_suma();
    for(register PolynomialMatrix::size_type i=1;i<=n_columns;i++)
      retval+= v1(1,i) * v2(i,1);
    return retval;  
  }
PolynomialMatrix operator*(const Polinomio &p,const m_double &m)
  {
    const m_double::size_type n_rows= m.getNumberOfRows();
    const m_double::size_type n_columns= m.getNumberOfColumns();
    PolynomialMatrix prod(n_rows,n_columns);
    for(register PolynomialMatrix::size_type i= 1;i<=n_rows;i++)
      for(register PolynomialMatrix::size_type j= 1;j<=n_columns;j++)
        prod(i,j)= m(i,j) * p;
    return prod;
  }
PolynomialMatrix operator*(const PolynomialMatrix &m,const double &d)
  {
    PolynomialMatrix retval(m);
    retval*=(d);
    return retval;
  }

PolynomialMatrix Primitiva(const PolynomialMatrix &m,short unsigned int k)
  {
    PolynomialMatrix retval(m);
    retval.Primitiva(k);
    return k;
  }

PolynomialMatrix Diferencial(const PolynomialMatrix &m, short unsigned int k)
  {
    PolynomialMatrix retval(m);
    retval.Diferencial(k);
    return k;
  }

PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const double &x0,const double &x1)
  {
    PolynomialMatrix p= Primitiva(m,j);
    return Eval(p,j,x1) - Eval(p,j,x0);
  }
PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const Polinomio &x0,const Polinomio &x1)
  {
    PolynomialMatrix p= Primitiva(m,j);
    return Eval(p,j,x1) - Eval(p,j,x0);
  }
PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const Polinomio &x0,const double &x1)
  {
    PolynomialMatrix p= Primitiva(m,j);
    return Eval(p,j,x1) - Eval(p,j,x0);
  }
PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const double &x0,const Polinomio &x1)
  {
    PolynomialMatrix p= Integral(m,j,x1,x0);
    return -p;
  }
PolynomialMatrix Derivada(const PolynomialMatrix &m,short unsigned int j,const double &x)
  { return Eval(Diferencial(m,j),j,x); }
PolynomialMatrix Derivada(const PolynomialMatrix &m,short unsigned int j,const Polinomio &x)
  { return Eval(Diferencial(m,j),j,x); }
