Index : sys / netinet / in.h == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =
                                                                                                                                    -- -sys / netinet / in.h(revision 344445)++ + sys / netinet / in.h(working copy) @ @-374,
                                                                                                                             7 + 374, 7 @ @

#define IN_LINKLOCAL(i) (((in_addr_t)(i)&0xffff0000) == 0xa9fe0000)
#define IN_LOOPBACK(i) (((in_addr_t)(i)&0xff000000) == 0x7f000000)
                                                                                                                                              -#define IN_ZERONET(i)(((in_addr_t)(i)&0xff000000) == 0) +
                                                                                                                                              #define IN_ZERONET(i)(((in_addr_t)(i)) == 0)

#define IN_PRIVATE(i)	((((in_addr_t)(i) & 0xff000000) == 0x0a000000) || \
 			 (((in_addr_t)(i) & 0xfff00000) == 0xac100000) || \
Index: sys/netinet/ip_icmp.c
                                                                                                                                          == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
                                                                                                                                      = -- -sys / netinet / ip_icmp.c(revision 344445)++ + sys / netinet / ip_icmp.c(working copy) @ @-733,
                                                                                                                             7 + 733, 6 @ @ int optlen = (ip->ip_hl << 2) - sizeof(struct ip);

if (IN_MULTICAST(ntohl(ip->ip_src.s_addr)) ||
    -IN_EXPERIMENTAL(ntohl(ip->ip_src.s_addr)) ||
    IN_ZERONET(ntohl(ip->ip_src.s_addr)))
{
    m_freem(m); /* Bad return address */
    ICMPSTAT_INC(icps_badaddr);
Index:
    sys / netpfil / ipfw / dn_sched_fq_codel_helper.h == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =
                                                                                                                                                             -- -sys / netpfil / ipfw / dn_sched_fq_codel_helper.h(revision 344445)++ + sys / netpfil / ipfw / dn_sched_fq_codel_helper.h(working copy) @ @-127,
                                                                                                                                                      7 + 127, 8 @ @ while (now >= cst->drop_next_time && cst->dropping)
    {

        /* mark the packet */
        -if (cprms->flags & CODEL_ECN_ENABLED && ecn_mark(m))
        {
            +if (cprms->flags & CODEL_ECN_ENABLED && sce_mark(m))
            {
                +printf("XXX\n");
                cst->count++;
                /* schedule the next mark. */
                cst->drop_next_time = control_law(cst, cprms, cst->drop_next_time);
            Index:
                sys / netpfil / ipfw / ip_dn_io.c == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =
                                                                                                                                                         -- -sys / netpfil / ipfw / ip_dn_io.c(revision 344445)++ + sys / netpfil / ipfw / ip_dn_io.c(working copy) @ @-430,
                                                                                                                                                  6 + 430, 61 @ @ static
#endif
                                                                                                                                                               int +
                                                                                                                                                               sce_mark(struct mbuf * m) +
                {
                    +struct ip *ip;
                    +ip = (struct ip *)mtodo(m, dn_tag_get(m)->iphdr_off);
                    + +switch (ip->ip_v)
                    {
                    +case IPVERSION:
                        +
                        {
                            +uint16_t old;
                            +if ((ip->ip_tos & IPTOS_ECN_MASK) == IPTOS_ECN_NOTECT) + return (0); /* not-ECT */
                            +if ((ip->ip_tos & IPTOS_ECN_MASK) == IPTOS_ECN_ECT1) + return (1);   /* already marked */
                            + +                                                                   /*
                                                                                 +                 * ecn-capable but not marked,
                                                                                 +                 * mark SCE and update checksum
                                                                                 +                 */
                              +old = *(uint16_t *)ip;
                            +ip->ip_tos &= ~IPTOS_ECN_CE;
                            +ip->ip_tos |= IPTOS_ECN_ECT1;
                            +ip->ip_sum = cksum_adjust(ip->ip_sum, old, *(uint16_t *)ip);
                            +return (1);
                            +
                        }
                    +#ifdef INET6 + case (IPV6_VERSION >> 4) :
                        +
                        {
                            +struct ip6_hdr *ip6 = (struct ip6_hdr *)ip;
                            +u_int32_t flowlabel;
                            + +flowlabel = ntohl(ip6->ip6_flow);
                            +if ((flowlabel >> 28) != 6) + return (0); /* version mismatch! */
                            +if ((flowlabel & (IPTOS_ECN_MASK << 20)) ==
                                 +(IPTOS_ECN_NOTECT << 20)) +
                                return (0); /* not-ECT */
                            +if ((flowlabel & (IPTOS_ECN_MASK << 20)) ==
                                 +(IPTOS_ECN_ECT1 << 20)) +
                                return (1); /* already marked */
                            +               /*
                           +                 * ecn-capable but not marked, mark CE
                           +                 */
                                +flowlabel |= (IPTOS_ECN_ECT1 << 20);
                            +ip6->ip6_flow = htonl(flowlabel);
                            +return (1);
                            +
                        }
                        +#endif +
                    }
                    +return (0);
                    +
                }
                + +#ifndef NEW_AQM + static + #endif + int ecn_mark(struct mbuf * m)
                {
                    struct ip *ip;
                Index:
                    sys / netpfil / ipfw / ip_dn_private.h == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =
                                                                                                                                                                  -- -sys / netpfil / ipfw / ip_dn_private.h(revision 344445)++ + sys / netpfil / ipfw / ip_dn_private.h(working copy) @ @-438,
                                                                                                                                                           6 + 438, 7 @ @

#ifdef NEW_AQM
                        int ecn_mark(struct mbuf * m);
                    +int sce_mark(struct mbuf * m);

                    /* moved from ip_dn_io.c to here to be available for AQMs modules*/
                    static inline void