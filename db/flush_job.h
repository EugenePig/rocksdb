//  Copyright (c) 2013, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
#pragma once

#include <atomic>
#include <deque>
#include <limits>
#include <set>
#include <utility>
#include <vector>
#include <string>

#include "db/dbformat.h"
#include "db/log_writer.h"
#include "db/snapshot.h"
#include "db/column_family.h"
#include "db/version_edit.h"
#include "db/memtable_list.h"
#include "port/port.h"
#include "rocksdb/db.h"
#include "rocksdb/env.h"
#include "rocksdb/memtablerep.h"
#include "rocksdb/transaction_log.h"
#include "util/autovector.h"
#include "util/stop_watch.h"
#include "util/thread_local.h"
#include "util/scoped_arena_iterator.h"
#include "db/internal_stats.h"
#include "db/write_controller.h"
#include "db/flush_scheduler.h"
#include "db/write_thread.h"
#include "db/job_context.h"

namespace rocksdb {

class MemTable;
class TableCache;
class Version;
class VersionEdit;
class VersionSet;
class Arena;

class FlushJob {
 public:
  // TODO(icanadi) make effort to reduce number of parameters here
  // IMPORTANT: mutable_cf_options needs to be alive while FlushJob is alive
  FlushJob(const std::string& dbname, ColumnFamilyData* cfd,
           const DBOptions& db_options,
           const MutableCFOptions& mutable_cf_options,
           const EnvOptions& env_options, VersionSet* versions,
           port::Mutex* db_mutex, std::atomic<bool>* shutting_down,
           FileNumToPathIdMap* pending_outputs, SequenceNumber newest_snapshot,
           JobContext* job_context, LogBuffer* log_buffer,
           Directory* db_directory, CompressionType output_compression,
           Statistics* stats);
  ~FlushJob() {}

  Status Run();

 private:
  Status WriteLevel0Table(const autovector<MemTable*>& mems, VersionEdit* edit,
                          uint64_t* filenumber);
  const std::string& dbname_;
  ColumnFamilyData* cfd_;
  const DBOptions& db_options_;
  const MutableCFOptions& mutable_cf_options_;
  const EnvOptions& env_options_;
  VersionSet* versions_;
  port::Mutex* db_mutex_;
  std::atomic<bool>* shutting_down_;
  FileNumToPathIdMap* pending_outputs_;
  SequenceNumber newest_snapshot_;
  JobContext* job_context_;
  LogBuffer* log_buffer_;
  Directory* db_directory_;
  CompressionType output_compression_;
  Statistics* stats_;
};

}  // namespace rocksdb
