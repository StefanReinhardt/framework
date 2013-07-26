#pragma once
#include <iostream>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

namespace composer
{
	namespace widgets
	{

		struct CategoryItem : public QWidget
		{
			Q_OBJECT
		public:
			CategoryItem( QString name, QWidget *content = 0, QWidget *parent = 0) : QWidget(parent), m_content(content)
			{
				if (objectName().isEmpty())
					setObjectName(QString::fromUtf8("CategoryItem"));
				

				m_verticalLayout = new QVBoxLayout(this);
				m_verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
				m_verticalLayout->setMargin(0);

				m_header = new QPushButton( this );
				m_header->setText( name );
				m_header->setMaximumHeight(20);

				m_verticalLayout->addWidget(m_header);

				if( content )
				{
					m_verticalLayout->addWidget(m_content);
					connect( m_content, SIGNAL(makeDirty()), this, SIGNAL(makeDirty()) );
				}

				connect( m_header, SIGNAL(clicked()), this, SLOT(toggleExpand()) );
			}


		public slots:
			void toggleExpand()
			{
				if( m_content )
				{
					if( m_content->isHidden() )
						m_content->show();
					else
						m_content->hide();
				}
			}

		signals:
			void makeDirty();

		private:
			QVBoxLayout *m_verticalLayout;
			QPushButton         *m_header;
			QWidget            *m_content;
		};

	}
}
