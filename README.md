# Treinplanner

A Sailfish OS app to search NS (Dutch Railways) stations and plan trips
between them. See `/home/readefries/.claude/plans/zippy-bubbling-deer.md` for
the full design rationale; this file covers what exists and how to build it.

## Status

Implemented (C++ backend + QML UI, not yet built/deployed - see "Build engine"
below):

- Station search is fully offline: `data/stations_nl.json` (bundled, ~750
  stations) is loaded and filtered in memory, no network call, no
  find-as-you-type (explicit submit only).
- Trip planning against `/api/v3/trips` (depart now / depart at / arrive by),
  with an offline check, a short-TTL anti-flicker cache, and a rolling
  5-minute request-budget tracker that warns before hitting the NS rate
  limit.
- API key handling: a user-entered key in Settings always wins; a build-time
  default key is only ever baked in for the OpenRepos/sideload variant (see
  "Build variants").
- Background station-list refresh (`StationBootstrap`): one `GET /stations`
  call, only when the local copy is >7 days old, online, and budget headroom
  is high.
- Cover page showing the next planned departure.

Not yet done: real icon assets (`icons/*/harbour-treinplanner.png` are
missing - packaging will fail without them), populated translations (the
`.ts` file is an empty skeleton), and any actual on-device/emulator testing.

## Authentication

Register at the [NS API portal](https://apiportal.ns.nl/) for a free
`Subscription-Key`. Every request needs it as a header.

## NS API endpoints used

- `GET /reisinformatie-api/api/v2/stations?limit=10&q={query}&countryCodes={countryCode}` -
  station search. `payload[].code` is the trip-planning key;
  `payload[].namen.lang/middel/kort` are display names of decreasing length.
- `GET /reisinformatie-api/api/v2/stations` (no `q`, no `limit`, no
  `countryCodes`) - confirmed to return the full list in one call (~750
  stations across NL and neighbouring countries NS serves). This is what
  `data/stations_nl.json` and `StationBootstrap` use.
- `GET /reisinformatie-api/api/v3/trips?fromStation={code}&toStation={code}&dateTime={iso8601}&departure={true|false}` -
  trip planning. `dateTime` is `yyyy-MM-ddTHH:mm:ss+HHMM` in the offset valid
  on the travel date (CET/CEST), not hardcoded - see `NsDateTime`.

## Regenerating the bundled station list

```
NS_API_KEY=<your key> ./scripts/fetch_stations.sh
```

Overwrites `data/stations_nl.json` with a fresh snapshot from the live API.

## Build variants

Both build from the same `harbour-treinplanner.pro` / source tree:

- **Harbour** (`rpm/harbour-treinplanner.yaml`): no default key baked in
  ever - BYO key is mandatory, guided via the Settings page. This is what
  gets submitted to the Jolla Harbour store.
- **OpenRepos/sideload** (`rpm/treinplanner-openrepos.yaml`): bakes in a
  default key at build time via `NS_API_DEFAULT_KEY`, kept out of git. A
  user-entered key in Settings always overrides it. Build with:

  ```
  sfdk qmake CONFIG+=openrepos_build
  NS_API_DEFAULT_KEY=<key> sfdk make
  sfdk package
  ```

  (Not a plain `sfdk build` - that runs the yaml-driven pipeline without a
  chance to inject `CONFIG+=openrepos_build` or the key.)

The Harbour variant just uses the normal pipeline:

```
sfdk build
```

## Build engine

This was developed and syntax-checked against the real Sailfish
`SailfishOS-5.1.0.11-aarch64` target headers (`sfdk` is installed), but
`sfdk` reported "No build engine found" in this environment (no
Docker/VirtualBox backend available), so nothing here has actually been
compiled, deployed, or run yet. All C++ files pass `g++ -fsyntax-only`
against the real target's Qt 5.6 headers; the QML has not been rendered or
tested at all. Both need a real build/deploy pass on your machine (or the
emulator) before trusting this beyond "should be structurally correct."

One assumption worth double-checking on first build: `NetworkStateMonitor`
uses `QNetworkConfigurationManager`, which the target sysroot does provide
(Qt 5.6.3), but it's a soft-deprecated API - confirm it still behaves as
expected on your target Sailfish OS version.
