#pragma once


#include <QtGui>
#include <QColor>
#include <QRgb>
#include <QColorDialog>

namespace composer
{

	class DoubleValidator : public QDoubleValidator 
	{ 
	public: 
	   DoubleValidator( double bottom, double top, int decimals, QObject* parent = 0) : QDoubleValidator( bottom, top, decimals, parent) 
	   {}
  
	   QValidator::State validate(QString & input, int &) const 
	   { 
		  const double b = bottom(); 
		  const double t = top(); 
		  const int d = decimals(); 

		  QRegExp empty(QString::fromLatin1("-?\\.?")); 
		  if (input.contains(' ')) 
			 return Invalid; 
		  if (b >= 0 && input.startsWith(QLatin1Char('-'))) 
			 return Invalid; 
		  if (empty.exactMatch(input)) 
			 return Intermediate; 

		  double entered = input.toDouble(); 
		  int nume = input.count('e', Qt::CaseInsensitive); 

		  int i;
		  if (input.contains(',')) 
			 i = input.indexOf(','); 
		  else 
			 i = input.indexOf('.'); 
    
		  if (i >= 0 && nume == 0) { 
			 i++; 
			 int j = i; 
			 while(input[j].isDigit()) 
				j++; 
			 if (j - i > d) 
				return Invalid; 
		  } 

		  if (entered < b || entered > t) 
			 return Invalid; 

		  return Acceptable;          
	   } 
	};


}