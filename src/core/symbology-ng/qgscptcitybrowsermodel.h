/***************************************************************************
    qgscptcitybrowsermodel.h
    ---------------------
    begin                : August 2012
    copyright            : (C) 2009 by Martin Dobias
    copyright            : (C) 2012 by Etienne Tourigny 
    email                : etourigny.dev at gmail.com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QGSCPTCITYBROWSERMODEL_H
#define QGSCPTCITYBROWSERMODEL_H

#include "qgsvectorcolorrampv2.h"

#include <QAbstractItemModel>
#include <QIcon>
#include <QMimeData>
#include <QAction>

class QgsCptCityColorRampV2;

/** base class for all items in the model */
class CORE_EXPORT QgsCptCityDataItem : public QObject
{
    Q_OBJECT
  public:
    enum Type
    {
      ColorRamp,
      Collection,
      Directory,
      Category
    };

    QgsCptCityDataItem( QgsCptCityDataItem::Type type, QgsCptCityDataItem* parent, 
                        QString name, QString path, QString info );
    virtual ~QgsCptCityDataItem();

    bool hasChildren();

    int rowCount();

    //

    virtual void refresh();

    // Create vector of children
    virtual QVector<QgsCptCityDataItem*> createChildren();

    // Populate children using children vector created by createChildren()
    virtual void populate();
    bool isPopulated() { return mPopulated; }

    // Insert new child using alphabetical order based on mName, emits necessary signal to model before and after, sets parent and connects signals
    // refresh - refresh populated item, emit signals to model
    virtual void addChildItem( QgsCptCityDataItem * child, bool refresh = false );

    // remove and delete child item, signals to browser are emited
    virtual void deleteChildItem( QgsCptCityDataItem * child );

    // remove child item but don't delete it, signals to browser are emited
    // returns pointer to the removed item or null if no such item was found
    virtual QgsCptCityDataItem * removeChildItem( QgsCptCityDataItem * child );

    virtual bool equal( const QgsCptCityDataItem *other );

    virtual QWidget * paramWidget() { return 0; }

    // list of actions provided by this item - usually used for popup menu on right-click
    virtual QList<QAction*> actions() { return QList<QAction*>(); }

    // whether accepts drag&drop'd layers - e.g. for import
    virtual bool acceptDrop() { return false; }

    // try to process the data dropped on this item
    virtual bool handleDrop( const QMimeData * /*data*/, Qt::DropAction /*action*/ ) { return false; }

     // static methods

    // Find child index in vector of items using '==' operator
    static int findItem( QVector<QgsCptCityDataItem*> items, QgsCptCityDataItem * item );
    static QgsCptCityDataItem* dataItem( QString path );

    // members

    Type type() const { return mType; }
    QgsCptCityDataItem* parent() const { return mParent; }
    void setParent( QgsCptCityDataItem* parent ) { mParent = parent; }
    QVector<QgsCptCityDataItem*> children() const { return mChildren; }
    QIcon icon() const { return mIcon; }
    QString name() const { return mName; }
    QString path() const { return mPath; }
    QString info() const { return mInfo; }

    void setIcon( QIcon icon ) { mIcon = icon; }

    void setToolTip( QString msg ) { mToolTip = msg; }
    QString toolTip() const { return mToolTip; }

    bool isValid() { return mValid; }

  protected:

    Type mType;
    QgsCptCityDataItem* mParent;
    QVector<QgsCptCityDataItem*> mChildren; // easier to have it always
    bool mPopulated;
    QString mName;
    QString mPath; // it is also used to identify item in tree
    QString mInfo;
    QString mToolTip;
    QIcon mIcon;
    bool mValid;

  public slots:
    void emitBeginInsertItems( QgsCptCityDataItem* parent, int first, int last );
    void emitEndInsertItems();
    void emitBeginRemoveItems( QgsCptCityDataItem* parent, int first, int last );
    void emitEndRemoveItems();

  signals:
    void beginInsertItems( QgsCptCityDataItem* parent, int first, int last );
    void endInsertItems();
    void beginRemoveItems( QgsCptCityDataItem* parent, int first, int last );
    void endRemoveItems();
};

/** Item that represents a layer that can be opened with one of the providers */
class CORE_EXPORT QgsCptCityColorRampItem : public QgsCptCityDataItem
{
    Q_OBJECT
  public:

    QgsCptCityColorRampItem( QgsCptCityDataItem* parent, 
                             QString name, QString path, QString info = QString(), 
                             QString variantName = QString() );

    // --- reimplemented from QgsCptCityDataItem ---

    virtual bool equal( const QgsCptCityDataItem *other );

    // --- New virtual methods for layer item derived classes ---
    const QgsCptCityColorRampV2& ramp() const { return mRamp; }

  protected:

    QgsCptCityColorRampV2 mRamp;
    QIcon mIcon;
};


/** A Collection: logical collection of subcollections and color ramps */
class CORE_EXPORT QgsCptCityCollectionItem : public QgsCptCityDataItem
{
    Q_OBJECT
  public:
    QgsCptCityCollectionItem( QgsCptCityDataItem* parent, 
                              QString name, QString path, QString info,
                              QString collectionName = DEFAULT_CPTCITY_COLLECTION );
    ~QgsCptCityCollectionItem();

    void setPopulated() { mPopulated = true; }
    void addChild( QgsCptCityDataItem *item ) { mChildren.append( item ); }

   protected:
    QString mCollectionName;

};

/** A directory: contains subdirectories and color ramps */
class CORE_EXPORT QgsCptCityDirectoryItem : public QgsCptCityCollectionItem
{
    Q_OBJECT
  public:
    QgsCptCityDirectoryItem( QgsCptCityDataItem* parent, 
                             QString name, QString path, QString info = QString(), 
                             QString collectionName = DEFAULT_CPTCITY_COLLECTION );
    ~QgsCptCityDirectoryItem();

    QVector<QgsCptCityDataItem*> createChildren();

    virtual bool equal( const QgsCptCityDataItem *other );

};

/** A category: contains subdirectories and color ramps */
class CORE_EXPORT QgsCptCityCategoryItem : public QgsCptCityCollectionItem
{
    Q_OBJECT
  public:
    QgsCptCityCategoryItem( QgsCptCityDataItem* parent, QString name, QString path, QString info,
                            QString collectionName = DEFAULT_CPTCITY_COLLECTION );
    ~QgsCptCityCategoryItem();

    QVector<QgsCptCityDataItem*> createChildren();

    virtual bool equal( const QgsCptCityDataItem *other );
};



class CORE_EXPORT QgsCptCityBrowserModel : public QAbstractItemModel
{
    Q_OBJECT

  public:
    explicit QgsCptCityBrowserModel( QObject *parent = 0, 
                                     QString collectionName = DEFAULT_CPTCITY_COLLECTION,
                                     QString viewName = "authors" );
    ~QgsCptCityBrowserModel();

    // implemented methods from QAbstractItemModel for read-only access

    /** Used by other components to obtain information about each item provided by the model.
      In many models, the combination of flags should include Qt::ItemIsEnabled and Qt::ItemIsSelectable. */
    virtual Qt::ItemFlags flags( const QModelIndex &index ) const;

    /** Used to supply item data to views and delegates. Generally, models only need to supply data
      for Qt::DisplayRole and any application-specific user roles, but it is also good practice
      to provide data for Qt::ToolTipRole, Qt::AccessibleTextRole, and Qt::AccessibleDescriptionRole.
      See the Qt::ItemDataRole enum documentation for information about the types associated with each role. */
    virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

    /** Provides views with information to show in their headers. The information is only retrieved
      by views that can display header information. */
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    /** Provides the number of rows of data exposed by the model. */
    virtual int rowCount( const QModelIndex &parent = QModelIndex() ) const;

    /** Provides the number of columns of data exposed by the model. List models do not provide this function
      because it is already implemented in QAbstractListModel. */
    virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const;

    /** Returns the index of the item in the model specified by the given row, column and parent index. */
    virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

    QModelIndex findItem( QgsCptCityDataItem *item, QgsCptCityDataItem *parent = 0 ) const;

    /** Returns the parent of the model item with the given index.
     * If the item has no parent, an invalid QModelIndex is returned.
     */
    virtual QModelIndex parent( const QModelIndex &index ) const;

    /** Returns a list of mime that can describe model indexes */
    /* virtual QStringList mimeTypes() const; */

    /** Returns an object that contains serialized items of data corresponding to the list of indexes specified */
    /* virtual QMimeData * mimeData( const QModelIndexList &indexes ) const; */

    /** Handles the data supplied by a drag and drop operation that ended with the given action */
    /* virtual bool dropMimeData( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent ); */

    QgsCptCityDataItem *dataItem( const QModelIndex &idx ) const;

    bool hasChildren( const QModelIndex &parent = QModelIndex() ) const;

    // Reload the whole model
    void reload();

    // Refresh item specified by path
    void refresh( QString path );

    // Refresh item childs
    void refresh( const QModelIndex &index = QModelIndex() );

    //! return index of a path
    QModelIndex findPath( QString path );

    void connectItem( QgsCptCityDataItem *item );

    bool canFetchMore( const QModelIndex & parent ) const;
    void fetchMore( const QModelIndex & parent );

  signals:

  public slots:
    //void removeItems( QgsCptCityDataItem * parent, QVector<QgsCptCityDataItem *>items );
    //void addItems( QgsCptCityDataItem * parent, QVector<QgsCptCityDataItem *>items );
    //void refreshItems( QgsCptCityDataItem * parent, QVector<QgsCptCityDataItem *>items );

    void beginInsertItems( QgsCptCityDataItem *parent, int first, int last );
    void endInsertItems();
    void beginRemoveItems( QgsCptCityDataItem *parent, int first, int last );
    void endRemoveItems();

  protected:

    // populates the model
    void addRootItems( );
    void removeRootItems();

    QVector<QgsCptCityDataItem*> mRootItems;
    QString mCollectionName;
    QString mViewName;
};

#endif