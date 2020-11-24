# netfilter - basic

- libnetfilter-queue-dev is required 

- To intecept incoming packets you can set ipables as NFQUEUE (e.g., ` iptables -A INPUT -j NFQUEUE`)
- To intecept incoming packets you can set ipables as NFQUEUE (e.g., ` iptables -A OUTPUT -j NFQUEUE`)
- To reset iptables, you can do ` iptables -F` 