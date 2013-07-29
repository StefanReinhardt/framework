#pragma once
#include <iostream>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include "CategoryItem.h"
namespace composer
{
	namespace widgets
	{

		struct CategoryList : public QWidget
		{
			Q_OBJECT
		public:
			CategoryList( QWidget *parent = 0) : QWidget(parent)
			{
				if (objectName().isEmpty())
					setObjectName(QString::fromUtf8("CategoryList"));

				m_verticalLayout = new QVBoxLayout(this);
				m_verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
				m_verticalLayout->setMargin(0);
				m_verticalLayout->setSpacing(0);
				m_verticalLayout->addStretch();

				numCategories = 0;
			}

			void addCategory( QString name, QWidget *content = 0 )
			{
				CategoryItem *cat = new CategoryItem( name, content);
				connect( cat, SIGNAL(makeDirty()), this, SIGNAL(makeDirty()) );
				m_verticalLayout->insertWidget( numCategories, cat );

				numCategories++;
			}

		public slots:

		signals:
			void makeDirty();


		private:
			QVBoxLayout *m_verticalLayout;
			int numCategories;
		};

	}
}
