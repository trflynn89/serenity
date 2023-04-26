/*
 * Copyright (c) 2023, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibSQL/AST/AST.h>
#include <LibSQL/Database.h>

namespace SQL::AST {

ResultOr<ResultSet> BeginTransaction::execute(ExecutionContext& context) const
{
    TRY(context.database->begin_transaction(context.connection_id));
    return ResultSet { SQLCommand::BeginTransaction };
}

ResultOr<ResultSet> CommitTransaction::execute(ExecutionContext& context) const
{
    TRY(context.database->commit_transaction());
    return ResultSet { SQLCommand::CommitTransaction };
}

}
