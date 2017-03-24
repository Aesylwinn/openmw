#include "pathgridcreator.hpp"

#include <QLabel>

#include "../../model/doc/document.hpp"

#include "../../model/world/columns.hpp"
#include "../../model/world/data.hpp"
#include "../../model/world/idcompletionmanager.hpp"
#include "../../model/world/idtable.hpp"

#include "../widget/droplineedit.hpp"
#include "idvalidator.hpp"

std::string CSVWorld::PathgridCreator::getId() const
{
    return mCell->text().toUtf8().constData();
}

CSMWorld::IdTable& CSVWorld::PathgridCreator::getPathgridsTable() const
{
    return dynamic_cast<CSMWorld::IdTable&> (
        *getData().getTableModel(getCollectionId())
    );
}

CSVWorld::PathgridCreator::PathgridCreator(
    CSMWorld::Data& data,
    QUndoStack& undoStack,
    const CSMWorld::UniversalId& id,
    CSMWorld::IdCompletionManager& completionManager,
    bool relaxedIdRules
) : GenericCreator(data, undoStack, id, relaxedIdRules)
{
    setManualEditing(false);

    QLabel *label = new QLabel("Cell ID", this);
    insertBeforeButtons(label, false);

    // Add cell ID input with auto-completion.
    CSMWorld::ColumnBase::Display displayType = CSMWorld::ColumnBase::Display_Cell;
    mCell = new CSVWidget::DropLineEdit(displayType, this);
    mCell->setCompleter(completionManager.getCompleter(displayType).get());
    mCell->setValidator(new IdValidator(relaxedIdRules, this));
    insertBeforeButtons(mCell, true);

    connect(mCell, SIGNAL (textChanged(const QString&)), this, SLOT (cellChanged()));
    connect(mCell, SIGNAL (returnPressed()), this, SLOT (inputReturnPressed()));
}

void CSVWorld::PathgridCreator::cloneMode(
    const std::string& originId,
    const CSMWorld::UniversalId::Type type)
{
    CSVWorld::GenericCreator::cloneMode(originId, type);

    // Look up cloned record in pathgrids table and set cell ID text.
    CSMWorld::IdTable& table = getPathgridsTable();
    int column = table.findColumnIndex(CSMWorld::Columns::ColumnId_Id);
    mCell->setText(table.data(table.getModelIndex(originId, column)).toString());
}

std::string CSVWorld::PathgridCreator::getErrors() const
{
    std::string cellId = getId();

    // Check user input for any errors.
    // The last two checks, cell with existing pathgrid and non-existent cell,
    // shouldn't be needed but we absolutely want to make sure they never happen.
    std::string errors;
    if (cellId.empty())
    {
        errors = "No cell ID selected";
    }
    else if (getData().getPathgrids().searchId(cellId) > -1)
    {
        errors = "Pathgrid for selected cell ID already exists";
    }
    else if (getData().getCells().searchId(cellId) == -1)
    {
        errors = "Cell with selected cell ID does not exist";
    }

    return errors;
}

void CSVWorld::PathgridCreator::focus()
{
    mCell->setFocus();
}

void CSVWorld::PathgridCreator::reset()
{
    CSVWorld::GenericCreator::reset();
    mCell->setText("");
}

void CSVWorld::PathgridCreator::cellChanged()
{
    update();
}

CSVWorld::Creator *CSVWorld::PathgridCreatorFactory::makeCreator(
    CSMDoc::Document& document,
    const CSMWorld::UniversalId& id) const
{
    return new PathgridCreator(
        document.getData(),
        document.getUndoStack(),
        id,
        document.getIdCompletionManager()
    );
}
