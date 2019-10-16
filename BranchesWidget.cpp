#include "BranchesWidget.h"

#include <BranchTreeWidget.h>
#include <git.h>
#include <BranchesViewDelegate.h>
#include <ClickableFrame.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QListWidget>
#include <QLabel>
#include <QMenu>
#include <QHeaderView>

#include <QLogger.h>

using namespace QLogger;

BranchesWidget::BranchesWidget(QSharedPointer<Git> git, QWidget *parent)
   : QWidget(parent)
   , mGit(git)
   , mLocalBranchesTree(new BranchTreeWidget(mGit))
   , mRemoteBranchesTree(new BranchTreeWidget(mGit))
   , mTagsList(new QListWidget())
   , mStashesList(new QListWidget())
   , mSubmodulesList(new QListWidget())
   , mTagsCount(new QLabel("(0)"))
   , mTagArrow(new QLabel())
   , mStashesArrow(new QLabel())
   , mSubmodulesCount(new QLabel("(0)"))
   , mSubmodulesArrow(new QLabel())
{
   mLocalBranchesTree->setLocalRepo(true);
   mLocalBranchesTree->setColumnHidden(0, true);
   mLocalBranchesTree->setMouseTracking(true);
   mLocalBranchesTree->setItemDelegate(new BranchesViewDelegate());
   mLocalBranchesTree->setColumnCount(4);

   const auto localHeader = mLocalBranchesTree->headerItem();
   localHeader->setText(1, QString("   %1").arg(tr("Local")));
   localHeader->setIcon(1, QIcon(":/icons/local"));
   localHeader->setText(2, tr("To master"));
   localHeader->setText(3, tr("To origin"));

   mRemoteBranchesTree->setColumnCount(2);
   mRemoteBranchesTree->setMouseTracking(true);
   mRemoteBranchesTree->setItemDelegate(new BranchesViewDelegate());
   mRemoteBranchesTree->setColumnHidden(0, true);

   const auto remoteHeader = mRemoteBranchesTree->headerItem();
   remoteHeader->setText(1, QString("   %1").arg(tr("Remote")));
   remoteHeader->setIcon(1, QIcon(":/icons/server"));

   /* TAGS */

   const auto tagsFrame = new ClickableFrame();
   tagsFrame->setObjectName("tagsFrame");
   const auto tagsHeaderLayout = new QHBoxLayout(tagsFrame);
   tagsHeaderLayout->setContentsMargins(20, 9, 10, 9);
   tagsHeaderLayout->setSpacing(10);
   QIcon icon(":/icons/tags");

   const auto tagsIcon = new QLabel();
   tagsIcon->setPixmap(icon.pixmap(QSize(15, 15)));

   tagsHeaderLayout->addWidget(tagsIcon);
   tagsHeaderLayout->addWidget(new QLabel(tr("Tags")));
   tagsHeaderLayout->addWidget(mTagsCount);
   tagsHeaderLayout->addStretch();

   icon = QIcon(":/icons/arrow_down");
   mTagArrow->setPixmap(icon.pixmap(QSize(15, 15)));
   tagsHeaderLayout->addWidget(mTagArrow);

   mTagsList->setMouseTracking(true);
   mTagsList->setContextMenuPolicy(Qt::CustomContextMenu);

   const auto tagLayout = new QVBoxLayout();
   tagLayout->setContentsMargins(QMargins());
   tagLayout->setSpacing(0);
   tagLayout->addWidget(tagsFrame);
   tagLayout->addWidget(mTagsList);

   /* TAGS END */

   /* STASHES */

   const auto stashFrame = new ClickableFrame();
   stashFrame->setObjectName("tagsFrame");

   const auto stashHeaderLayout = new QHBoxLayout(stashFrame);
   stashHeaderLayout->setContentsMargins(20, 9, 10, 9);
   stashHeaderLayout->setSpacing(10);

   QIcon stashIcon(":/icons/stashes");

   const auto stashIconLabel = new QLabel();
   stashIconLabel->setPixmap(stashIcon.pixmap(QSize(15, 15)));

   stashHeaderLayout->addWidget(stashIconLabel);
   stashHeaderLayout->addWidget(new QLabel(tr("Stashes")));
   stashHeaderLayout->addWidget(mStashesCount = new QLabel(tr("(0)")));
   stashHeaderLayout->addStretch();

   stashIcon = QIcon(":/icons/arrow_down");
   mStashesArrow->setPixmap(stashIcon.pixmap(QSize(15, 15)));
   stashHeaderLayout->addWidget(mStashesArrow);

   mStashesList->setMouseTracking(true);
   mStashesList->setContextMenuPolicy(Qt::CustomContextMenu);

   const auto stashLayout = new QVBoxLayout();
   stashLayout->setContentsMargins(QMargins());
   stashLayout->setSpacing(0);
   stashLayout->addWidget(stashFrame);
   stashLayout->addWidget(mStashesList);

   /* STASHES END */

   const auto submoduleFrame = new ClickableFrame();
   submoduleFrame->setObjectName("tagsFrame");

   const auto submoduleHeaderLayout = new QHBoxLayout(submoduleFrame);
   submoduleHeaderLayout->setContentsMargins(20, 9, 10, 9);
   submoduleHeaderLayout->setSpacing(10);

   QIcon submoduleIcon(":/icons/submodules");
   const auto submoduleIconLabel = new QLabel();
   submoduleIconLabel->setPixmap(submoduleIcon.pixmap(QSize(15, 15)));

   submoduleHeaderLayout->addWidget(submoduleIconLabel);
   submoduleHeaderLayout->addWidget(new QLabel(tr("Submodules")));
   submoduleHeaderLayout->addWidget(mSubmodulesCount);
   submoduleHeaderLayout->addStretch();

   submoduleIcon = QIcon(":/icons/arrow_down");
   mSubmodulesArrow->setPixmap(submoduleIcon.pixmap(QSize(15, 15)));

   submoduleHeaderLayout->addWidget(mSubmodulesArrow);

   mSubmodulesList->setMouseTracking(true);
   mSubmodulesList->setContextMenuPolicy(Qt::CustomContextMenu);

   const auto submoduleLayout = new QVBoxLayout();
   submoduleLayout->setContentsMargins(QMargins());
   submoduleLayout->setSpacing(0);
   submoduleLayout->addWidget(submoduleFrame);
   submoduleLayout->addWidget(mSubmodulesList);

   /* SUBMODULES START */

   /* SUBMODULES END */

   const auto vLayout = new QVBoxLayout(this);
   vLayout->setContentsMargins(QMargins());
   vLayout->addWidget(mLocalBranchesTree);
   vLayout->addWidget(mRemoteBranchesTree);
   vLayout->addLayout(tagLayout);
   vLayout->addLayout(stashLayout);
   vLayout->addLayout(submoduleLayout);

   setLayout(vLayout);

   connect(mLocalBranchesTree, &BranchTreeWidget::signalSelectCommit, this, &BranchesWidget::signalSelectCommit);
   connect(mLocalBranchesTree, &BranchTreeWidget::signalBranchesUpdated, this, &BranchesWidget::signalBranchesUpdated);
   connect(mRemoteBranchesTree, &BranchTreeWidget::signalSelectCommit, this, &BranchesWidget::signalSelectCommit);
   connect(mRemoteBranchesTree, &BranchTreeWidget::signalBranchesUpdated, this, &BranchesWidget::signalBranchesUpdated);
   connect(mTagsList, &QListWidget::itemClicked, this, &BranchesWidget::onTagClicked);
   connect(mTagsList, &QListWidget::customContextMenuRequested, this, &BranchesWidget::showTagsContextMenu);
   connect(mTagsList, &QListWidget::customContextMenuRequested, this, &BranchesWidget::showTagsContextMenu);
   connect(mStashesList, &QListWidget::itemClicked, this, &BranchesWidget::onStashClicked);
   connect(mStashesList, &QListWidget::customContextMenuRequested, this, &BranchesWidget::showStashesContextMenu);
   connect(mSubmodulesList, &QListWidget::customContextMenuRequested, this, &BranchesWidget::showSubmodulesContextMenu);
   connect(tagsFrame, &ClickableFrame::clicked, this, &BranchesWidget::onTagsHeaderClicked);
   connect(stashFrame, &ClickableFrame::clicked, this, &BranchesWidget::onStashesHeaderClicked);
   connect(submoduleFrame, &ClickableFrame::clicked, this, &BranchesWidget::onSubmodulesHeaderClicked);
}

void BranchesWidget::showBranches()
{
   QLog_Info("UI", QString("Loading branches data"));

   clear();

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   auto ret = mGit->getBranches();

   if (ret.success)
   {
      auto output = ret.output.toString();
      if (!output.startsWith("fatal"))
      {
         output.replace(' ', "");
         const auto branches = output.split('\n');

         QLog_Info("UI", QString("Fetched {%1} branches").arg(branches.count()));
         QLog_Info("UI", QString("Processing branches...").arg(branches.count()));

         for (auto branch : branches)
         {
            if (!branch.isEmpty())
            {
               if (branch.startsWith("remotes/") && !branch.contains("HEAD->"))
                  processRemoteBranch(branch);
               else if (!branch.contains("HEAD->"))
                  processLocalBranch(branch);
            }
         }

         QLog_Info("UI", QString("... branches processed").arg(branches.count()));
      }

      processTags();
      processStashes();
      processSubmodules();

      QApplication::restoreOverrideCursor();

      adjustBranchesTree(mLocalBranchesTree);
      adjustBranchesTree(mRemoteBranchesTree);
   }
}

void BranchesWidget::clear()
{
   blockSignals(true);
   mLocalBranchesTree->clear();
   mRemoteBranchesTree->clear();
   mTagsList->clear();
   mStashesList->clear();
   mSubmodulesList->clear();
   blockSignals(false);
}

void BranchesWidget::processLocalBranch(QString branch)
{
   auto item = new QTreeWidgetItem(mLocalBranchesTree);
   item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);

   if (branch.startsWith('*'))
   {
      branch.replace('*', "");
      item->setData(1, Qt::UserRole, true);
   }

   QLog_Debug("UI", QString("Adding local branch {%1}").arg(branch));

   QLog_Debug("UI", QString("Calculating distances..."));

   auto distance = mGit->getDistanceBetweenBranches(true, branch).output.toString();
   distance.replace('\n', "");
   distance.replace('\t', "\u2193 - ");
   distance.append("\u2191");
   item->setText(2, distance);

   distance = mGit->getDistanceBetweenBranches(false, branch).output.toString();

   if (!distance.contains("fatal"))
   {
      distance.replace('\n', "");
      distance.append("\u2191");
   }
   else
      distance = "Local";

   item->setText(3, distance);
   item->setText(1, branch);

   mLocalBranchesTree->addTopLevelItem(item);

   QLog_Debug("UI", QString("Finish gathering local branch information"));
}

void BranchesWidget::processRemoteBranch(QString branch)
{
   branch.replace("remotes/", "");

   QLog_Debug("UI", QString("Adding remote branch {%1}").arg(branch));

   auto item = new QTreeWidgetItem(mRemoteBranchesTree);
   item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);

   branch.replace("origin/", "");

   item->setText(1, branch);

   mRemoteBranchesTree->addTopLevelItem(item);
}

void BranchesWidget::processTags()
{
   const auto tags = mGit->getTags();

   QLog_Info("UI", QString("Fetching {%1} tags").arg(tags.count()));

   for (auto tag : tags)
      mTagsList->addItem(tag);

   mTagsCount->setText(QString("(%1)").arg(tags.count()));
}

void BranchesWidget::processStashes()
{
   const auto stashes = mGit->getStashes();

   QLog_Info("UI", QString("Fetching {%1} stashes").arg(stashes.count()));

   for (auto stash : stashes)
   {
      const auto stashId = stash.split(":").first();
      const auto stashDesc = stash.split("}: ").last();
      auto item = new QListWidgetItem(stashDesc);
      item->setData(Qt::UserRole, stashId);
      mStashesList->addItem(item);
   }

   mStashesCount->setText(QString("(%1)").arg(stashes.count()));
}

void BranchesWidget::processSubmodules()
{
   const auto submodules = mGit->getSubmodules();

   QLog_Info("UI", QString("Fetching {%1} submodules").arg(submodules.count()));

   for (auto submodule : submodules)
      mSubmodulesList->addItem(submodule);
}

void BranchesWidget::adjustBranchesTree(BranchTreeWidget *treeWidget)
{
   for (auto i = 2; i < treeWidget->columnCount(); ++i)
      treeWidget->resizeColumnToContents(i);

   treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);

   for (auto i = 2; i < treeWidget->columnCount(); ++i)
      treeWidget->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);

   treeWidget->header()->setStretchLastSection(false);
}

void BranchesWidget::showTagsContextMenu(const QPoint &p)
{
   QModelIndex index = mTagsList->indexAt(p);

   if (!index.isValid())
      return;

   const auto tagName = index.data().toString();
   const auto menu = new QMenu(this);
   const auto removeTagAction = menu->addAction(tr("Remove tag"));
   connect(removeTagAction, &QAction::triggered, this, [this, tagName]() {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      const auto ret = mGit->removeTag(tagName, true);
      QApplication::restoreOverrideCursor();

      if (ret)
         emit signalBranchesUpdated();
   });

   menu->exec(mTagsList->viewport()->mapToGlobal(p));
}

void BranchesWidget::showStashesContextMenu(const QPoint &) {}

void BranchesWidget::showSubmodulesContextMenu(const QPoint &p)
{
   QModelIndex index = mSubmodulesList->indexAt(p);

   if (!index.isValid())
      return;

   const auto submoduleName = index.data().toString();
   const auto menu = new QMenu(this);
   const auto updateSubmoduleAction = menu->addAction(tr("Update"));
   connect(updateSubmoduleAction, &QAction::triggered, this, [this, submoduleName]() {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      QByteArray output;
      const auto ret = mGit->submoduleUpdate(submoduleName);
      QApplication::restoreOverrideCursor();

      if (ret)
         emit signalBranchesUpdated();
   });

   const auto openSubmoduleAction = menu->addAction(tr("Open"));
   connect(openSubmoduleAction, &QAction::triggered, this,
           [this, submoduleName]() { emit signalOpenSubmodule(submoduleName); });

   const auto deleteSubmoduleAction = menu->addAction(tr("Delete"));
   connect(deleteSubmoduleAction, &QAction::triggered, this, []() {});

   menu->exec(mSubmodulesList->viewport()->mapToGlobal(p));
}

void BranchesWidget::onTagsHeaderClicked()
{
   const auto tagsAreVisible = mTagsList->isVisible();
   const auto icon = QIcon(tagsAreVisible ? QString(":/icons/arrow_up") : QString(":/icons/arrow_down"));
   mTagArrow->setPixmap(icon.pixmap(QSize(15, 15)));
   mTagsList->setVisible(!tagsAreVisible);
}

void BranchesWidget::onStashesHeaderClicked()
{
   const auto stashesAreVisible = mStashesList->isVisible();
   const auto icon = QIcon(stashesAreVisible ? QString(":/icons/arrow_up") : QString(":/icons/arrow_down"));
   mStashesArrow->setPixmap(icon.pixmap(QSize(15, 15)));
   mStashesList->setVisible(!stashesAreVisible);
}

void BranchesWidget::onSubmodulesHeaderClicked()
{
   const auto submodulesAreVisible = mSubmodulesList->isVisible();
   const auto icon = QIcon(submodulesAreVisible ? QString(":/icons/arrow_up") : QString(":/icons/arrow_down"));
   mSubmodulesArrow->setPixmap(icon.pixmap(QSize(15, 15)));
   mSubmodulesList->setVisible(!submodulesAreVisible);
}

void BranchesWidget::onTagClicked(QListWidgetItem *item)
{
   QByteArray sha;
   mGit->getTagCommit(item->text(), sha);

   emit signalSelectCommit(QString::fromUtf8(sha));
}

void BranchesWidget::onStashClicked(QListWidgetItem *item)
{
   QByteArray sha;
   mGit->getTagCommit(item->data(Qt::UserRole).toString(), sha);

   emit signalSelectCommit(QString::fromUtf8(sha));
}
