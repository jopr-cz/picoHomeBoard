#!/bin/bash

# Parametry Modbus spojení
DEVICE="/dev/ttyUSB0"  # Modbus RTU zařízení
BAUDRATE=9600           # Rychlost přenosu
SLAVE_ID=1              # ID Modbus zařízení
TCP_PORT=5020           # TCP port pro Modbus spojení

# Funkce pro načtení registrů
read_registers() {
    local start_addr=$1  # Počáteční adresa registrů
    local count=$2       # Počet registrů k načtení

    # Přičtení 1 k adrese pro modpoll (modpoll startuje od 1 místo 0)
    local modpoll_start_addr=$((start_addr + 1))

    # Použití modpoll pro čtení registrů z Modbus zařízení
    ./modpoll -m enc -a $SLAVE_ID -r $modpoll_start_addr -c $count -1 -p $TCP_PORT 127.0.0.1
}

# Funkce pro extrakci hodnot z výstupu modpoll
extract_values() {
    # Výstup modpoll je hledán pomocí regulárního výrazu "[x]: value".
    echo "$1" | awk -F']:' '/\[/{gsub(/^ +| +$/,"",$2); print $2}'
}

# Čtení hodnot z Modbus registrů pomocí funkcí
POS_VALUES_RAW=$(read_registers 0x00 6)          # Pozice žaluzie (6 registrů od adresy 0x00)
SHUTTER_VALUES_RAW=$(read_registers 0x10 6)      # Pozice shutter (6 registrů od adresy 0x10)
MAX_DOWN_VALUES_RAW=$(read_registers 0x100 6)    # maxDownTime (6 registrů od adresy 0x100)
MAX_SHUTTER_VALUES_RAW=$(read_registers 0x110 6) # maxShutterTime (6 registrů od adresy 0x110)

# Extrakce hodnot z jednotlivých kategorií
POS_VALUES=$(extract_values "$POS_VALUES_RAW")
SHUTTER_VALUES=$(extract_values "$SHUTTER_VALUES_RAW")
MAX_DOWN_VALUES=$(extract_values "$MAX_DOWN_VALUES_RAW")
MAX_SHUTTER_VALUES=$(extract_values "$MAX_SHUTTER_VALUES_RAW")

# Výpis hlavičky tabulky
echo -e "\n\033[1m| Zaluzie | Pozice [%] | Shutter [%] | Max Down [ms] | Max Shutter [ms] |\033[0m"
echo "|---------|-------------|--------------|---------------|----------------|"

# Výpis hodnot do tabulky
for i in {1..6}; do
    pos=$(echo "$POS_VALUES" | sed -n "${i}p")         # Hodnota pozice žaluzie
    shutter=$(echo "$SHUTTER_VALUES" | sed -n "${i}p") # Hodnota shutter
    max_down=$(echo "$MAX_DOWN_VALUES" | sed -n "${i}p") # maxDownTime
    max_shutter=$(echo "$MAX_SHUTTER_VALUES" | sed -n "${i}p") # maxShutterTime

    # Formátovaný výstup do tabulky
    printf "| %7d | %11d | %12d | %13d | %14d |\n" $i $pos $shutter $max_down $max_shutter
done
