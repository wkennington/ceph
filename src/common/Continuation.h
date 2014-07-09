// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2014 Red Hat
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include "include/Context.h"
#include <map>

class Continuation {
  int serial_stage;
  int rval;
  Context *on_finish;
  class Callback : public Context {
    Continuation *continuation;
    int stage_to_activate;
  public:
    Callback(Continuation *c, int stage) :
      continuation(c),
      stage_to_activate(stage) {}
    void finish(int r) {
      continuation->continue_function(r, stage_to_activate);
    }
  };
  protected:
  virtual bool _continue_function(int r, int stage) = 0;
  private:
  void continue_function(int r, int stage) {
    bool done = _continue_function(r, stage);
    if (done) {
      on_finish->complete(rval);
      on_finish = NULL;
      delete this;
    }
  }

public:
  Continuation(Context *c) : serial_stage(0), rval(0), on_finish(c) {}
  virtual ~Continuation() { assert(on_finish == NULL); }

  void begin() { continue_function(0, 0); }

  Context *get_serial_callback() {
    ++serial_stage;
    return new Callback(this, serial_stage);
  }
  int get_serial_stage() { return serial_stage; }
  void skip_serial_stage(int skip_stage) {
    assert(skip_stage == ++serial_stage);
  }
  void set_rval(int new_rval) { rval = new_rval; }
};
