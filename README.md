# libopeninv

## About this fork

This is the moltenmosfet fork of jsphuebner/libopeninv. It exists as a submodule of [moltenmosfet/stm32-sine](https://github.com/moltenmosfet/stm32-sine), a fork of the Open Inverter firmware adapted to drive a Nissan Leaf EM57 motor as a chassis-dyno absorber.

`dyno-main` is the integration branch, created off the pinned baseline commit 78e3f72. Fix branches (`fix/T<nn>-…`) merge into it.

Most changes are bug fixes intended for upstream (jsphuebner) as a PR series. Per-change notes live in the superproject's [FORK_NOTES.md](https://github.com/moltenmosfet/stm32-sine/blob/dyno-main/FORK_NOTES.md).

Nothing here is bench-validated on hardware yet. Changes are host-tested and compile-checked only.

[![Build status](../../actions/workflows/CI-build.yml/badge.svg)](../../actions/workflows/CI-build.yml)

Generic modules that can be used in many projects
