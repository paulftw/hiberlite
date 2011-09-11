#ifndef _HIBERLITE_H_
#define _HIBERLITE_H_

#include <sqlite3.h>

#include "common.h"
#include "shared_res.h"
#include "hiberdefs.h"
#include "nvp.h"

#include "db_error.h"

#include "SQLiteStmt.h"

#include "CppModel.h"

#include "Registry.h"
#include "bean_ptr.h"

#include "Visitor.h"
#include "ModelExtractor.h"
#include "BeanUpdater.h"
#include "ChildKiller.h"
#include "BeanLoader.h"

#include "Database.h"


#include "Registry_impl.hpp"
#include "bean_ptr_impl.hpp"
#include "Database_tmpl_impl.hpp"

#include "Visitor_tmpl_impl.hpp"
#include "ModelExtractor_impl.hpp"
#include "BeanUpdater_impl.hpp"
#include "ChildKiller_impl.hpp"
#include "BeanLoader_impl.hpp"

#include "some_types.h"

#endif //_HIBERLITE_H_
