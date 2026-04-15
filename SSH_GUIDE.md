# PiRacer Head Unit – SSH Connection Guide

## Device Identity

| Item | Value |
|---|---|
| Hostname | `raspberrypi4-64` |
| MAC Address (wlan0) | `d8:3a:dd:fa:68:ce` |
| SSH User | `root` |
| SSH Password | *(none – debug-tweaks image)* |
| Network | SEA:ME WiFi Access |

> The IP address changes on every boot (DHCP). Always find it by MAC address.

---

## Step 1 – Find the IP Address

### Recommended: filter by MAC address

```bash
# Scan the subnet (replace 192.168.86 if your network is different)
arp -n | grep d8:3a:dd:fa:68:ce
```

If `arp` table is empty (Pi just booted), trigger an nmap scan first:

```bash
nmap -sn 192.168.86.0/24 > /dev/null && arp -n | grep d8:3a:dd:fa:68:ce
```

Expected output:
```
192.168.86.XX  ether  d8:3a:dd:fa:68:ce  C  wlp0s20f3
```

### Alternative: one-liner to get the IP directly

```bash
nmap -sn 192.168.86.0/24 > /dev/null && arp -n | grep "d8:3a:dd:fa:68:ce" | awk '{print $1}'
```

---

## Step 2 – Connect via SSH

```bash
ssh root@<IP>
# Example:
ssh root@192.168.86.31
```

No password required. You will see:

```
WARNING: Poky is a reference Yocto Project distribution...
root@raspberrypi4-64:~#
```

---

## Step 3 – Verify It's Our Device

```bash
# Check hostname
hostname
# → raspberrypi4-64

# Check our services are running
ps | grep hu_shell
# → /usr/bin/hu_shell and all modules should appear

# Check WiFi IP
ip addr show wlan0
```

---

## Quick One-Liner (find + connect)

```bash
PI_IP=$(nmap -sn 192.168.86.0/24 > /dev/null && arp -n | grep "d8:3a:dd:fa:68:ce" | awk '{print $1}') && ssh root@$PI_IP
```

---

## Troubleshooting

| Problem | Cause | Fix |
|---|---|---|
| MAC not in `arp` table | Pi still booting / WiFi not up | Wait 30s and retry |
| `arp` returns nothing after nmap | Pi not on WiFi | Connect ethernet cable and use `ssh root@<eth0 IP>` |
| SSH asks for password | Different Pi (someone else's) | Wrong device – find ours by MAC |
| `nmap` finds no new host | DHCP not assigned yet | Rerun nmap after 1 minute |

### If WiFi fails completely – use Ethernet

```bash
# Plug ethernet cable into Pi
# Rescan – eth0 will appear as a new IP
nmap -sn 192.168.86.0/24 > /dev/null && arp -n | grep "d8:3a:dd:fa:68:ce" | awk '{print $1}'
```

---

## Useful Commands on the Pi (BusyBox shell)

```bash
ps | grep hu          # check head unit processes
ip addr               # show all network interfaces
cat /etc/wpa_supplicant.conf   # check WiFi config
logread | tail -50    # system logs (no journalctl)
```

> **Note:** This image uses SysVinit, not systemd. `systemctl` is not available. Use `logread` instead of `journalctl`.
