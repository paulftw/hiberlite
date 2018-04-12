cmake_minimum_required(VERSION 3.6)

project(hiberlite)

#find_package(Qt5 CONFIG REQUIRED Core Gui Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-terminate")

set(TARGET_NAME hiberlite) 

set(SOURCES_SQLITE
  sqlite-amalgamation/sqlite3.c
  sqlite-amalgamation/sqlite3.h
)
  
set(SOURCES  
  src/BeanLoader.cpp
  src/BeanUpdater.cpp
  src/ChildKiller.cpp
  src/CppModel.cpp
  src/Database.cpp
  src/ModelExtractor.cpp
  src/Registry.cpp
  src/shared_res.cpp
  src/SQLiteStmt.cpp
  src/Visitor.cpp

  include/BeanLoader.h
  include/BeanLoader_impl.hpp
  include/BeanUpdater.h
  include/BeanUpdater_impl.hpp
  include/ChildKiller.h
  include/ChildKiller_impl.hpp
  include/CppModel.h
  include/Database.h
  include/Database_tmpl_impl.hpp
  include/ModelExtractor.h
  include/ModelExtractor_impl.hpp
  include/Registry.h
  include/Registry_impl.hpp
  include/SQLiteStmt.h
  include/UpdateVisitor_tmpl.hpp
  include/Visitor.h
  include/Visitor_tmpl_impl.hpp
  include/bean_ptr.h
  include/bean_ptr_impl.hpp
  include/common.h
  include/db_error.h
  include/hiberdefs.h
  include/hiberlite.h
  include/nvp.h
  include/shared_res.h
  include/some_types.h
  )

add_library(${TARGET_NAME}
  ${SOURCES}
  ${SOURCES_SQLITE}
)

target_include_directories(${TARGET_NAME}
  PUBLIC
  include
  sqlite-amalgamation
)

target_link_libraries(${TARGET_NAME}
  pthread
  dl
  )

add_executable(sample
  sample.cpp
  )

target_link_libraries(sample
  ${TARGET_NAME}
  )

add_executable(tests
  tests.cpp
  )

target_link_libraries(tests
  ${TARGET_NAME}
)
  
