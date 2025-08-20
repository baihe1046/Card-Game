#pragma once
#include <stack>
#include "services/OperationRecord.h"


class UndoManager {
public:
    void pushRecord(const OperationRecord& record);
    OperationRecord popRecord();
    bool isEmpty() const;
    void clear();

private:
    std::stack<OperationRecord> _undoStack;
};
