#/bin/bash

for i in $(psql -U postgres -d sniffy \
  --single-transaction \
  -c 'set search_path = ag_catalog, "$user", public;' \
  -f nslookup_traffic_via_protocol_22.sql \
| awk '{print $1 }' | tail -n +4 | head -n -2 \
)
do
  echo "[$i]"
  nslookup $i
done

