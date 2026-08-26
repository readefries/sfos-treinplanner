#!/bin/sh
# Regenerates data/stations_nl.json from the live NS API.
#
# Confirmed behaviour (2026-08-26): GET /stations with no `q` and no `limit`
# returns the full station list in a single request (~750 stations across
# NL and neighbouring countries NS serves) - no pagination needed.
#
# Usage: NS_API_KEY=<your subscription key> ./scripts/fetch_stations.sh

set -eu

if [ -z "${NS_API_KEY:-}" ]; then
    echo "Set NS_API_KEY to your NS API portal subscription key first." >&2
    exit 1
fi

cd "$(dirname "$0")/.."

curl -sSf \
    -H "Subscription-Key: ${NS_API_KEY}" \
    'https://gateway.apiportal.ns.nl/reisinformatie-api/api/v2/stations' \
    -o data/stations_nl.json

echo "Wrote data/stations_nl.json ($(wc -c < data/stations_nl.json) bytes)"
