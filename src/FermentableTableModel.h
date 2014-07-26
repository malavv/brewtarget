/*
 * FermentableTableModel.h is part of Brewtarget, and is Copyright Philip G. Lee
 * (rocketman768@gmail.com), 2009-2013.
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _FERMENTABLETABLEMODEL_H
#define   _FERMENTABLETABLEMODEL_H

class FermentableTableModel;
class FermentableItemDelegate;

#include <QAbstractTableModel>
#include <QTableView>
#include <QWidget>
#include <QModelIndex>
#include <QVariant>
#include <QMetaProperty>
#include <QItemDelegate>
#include <QAbstractItemDelegate>
#include <QList>
#include "unit.h"

// Forward declarations.
class Fermentable;
class Recipe;

enum{FERMNAMECOL, FERMTYPECOL, FERMINVENTORYCOL, FERMAMOUNTCOL, FERMISMASHEDCOL, FERMAFTERBOIL, FERMYIELDCOL, FERMCOLORCOL, FERMNUMCOLS /*This one MUST be last*/};

/*!
 * \class FermentableTableModel
 * \author Philip G. Lee
 *
 * \brief A table model for a list of fermentables.
 */
class FermentableTableModel : public QAbstractTableModel
{
   Q_OBJECT

public:
   FermentableTableModel(QTableView* parent=0, bool editable=true);
   virtual ~FermentableTableModel() {}
   //! \brief Observe a recipe's list of fermentables.
   void observeRecipe(Recipe* rec);
   //! \brief If true, we model the database's list of fermentables.
   void observeDatabase(bool val);
   //! \brief Watch all the \b ferms for changes.
   void addFermentables(QList<Fermentable*> ferms);
   //! \brief Clear the model.
   void removeAll();
   //! \brief Return the \c i-th fermentable in the model.
   Fermentable* getFermentable(unsigned int i);
   //! \brief True if you want to display percent of each grain in the row header.
   void setDisplayPercentages( bool var );

   unitDisplay displayUnit(int column) const;
   unitScale displayScale(int column) const;
   void setDisplayUnit(int column, unitDisplay displayUnit);
   void setDisplayScale(int column, unitScale displayScale);

   //! \brief Reimplemented from QAbstractTableModel.
   virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
   //! \brief Reimplemented from QAbstractTableModel.
   virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
   //! \brief Reimplemented from QAbstractTableModel.
   virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
   //! \brief Reimplemented from QAbstractTableModel.
   virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
   //! \brief Reimplemented from QAbstractTableModel.
   virtual Qt::ItemFlags flags(const QModelIndex& index ) const;
   //! \brief Reimplemented from QAbstractTableModel.
   virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
   
   QTableView* parentTableWidget;
   
public slots:
   //! \brief Watch \b ferm for changes.
   void addFermentable(Fermentable* ferm);
   //! \returns true if "ferm" is successfully found and removed.
   bool removeFermentable(Fermentable* ferm);

private slots:
   //! \brief Catch changes to Recipe, Database, and Fermentable.
   void changed(QMetaProperty, QVariant);

private:
   //! \brief Recalculate the total amount of grains in the model.
   void updateTotalGrains();
   QString generateName(int column) const;
   
   bool editable;
   QList<Fermentable*> fermObs;
   Recipe* recObs;
   bool displayPercentages;
   double totalFermMass_kg;
   
};

/*!
 * \brief An item delegate for Fermentable tables.
 * \sa FermentableTableModel.
 * 
 * \author Philip G. Lee
 */
class FermentableItemDelegate : public QItemDelegate
{
   Q_OBJECT
           
public:
   FermentableItemDelegate(QObject* parent = 0);
   
   //! \brief Reimplemented from QItemDelegate.
   virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
   //! \brief Reimplemented from QItemDelegate.
   virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
   //! \brief Reimplemented from QItemDelegate.
   virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
   //! \brief Reimplemented from QItemDelegate.
   virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
   //virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
   
//public slots:
//   void destroyWidget(QWidget* widget, QAbstractItemDelegate::EndEditHint hint);
private:
};

#endif   /* _FERMENTABLETABLEMODEL_H */

