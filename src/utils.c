void print_header() {
    printf("+----------+-----+---+---+--------+--------+------------+------------+------------+-----------------------+\n");
    printf("| impl     | cap | P | C | items  | trials | time_avg_s | time_min_s | time_max_s | throughput_ops_per_s |\n");
    printf("+----------+-----+---+---+--------+--------+------------+------------+------------+-----------------------+\n");
}

void print_header_seq() {
    printf("+----------+-----+--------+--------+------------+------------+------------+-----------------------+\n");
    printf("| impl     | cap | items  | trials | time_avg_s | time_min_s | time_max_s | throughput_ops_per_s |\n");
    printf("+----------+-----+--------+--------+------------+------------+------------+-----------------------+\n");
}

void print_row(const char *impl, int cap, int P, int C, int items, int t,
               double avg, double tmin, double tmax, double thr) {
    printf("| %-8s | %3d | %1d | %1d | %6d | %6d | %10.4f | %10.4f | %10.4f | %21.1f |\n",
           impl, cap, P, C, items, t, avg, tmin, tmax, thr);
}

void print_row_seq(const char *impl, int cap, int items, int t,
                   double avg, double tmin, double tmax, double thr) {
    printf("| %-8s | %3d | %6d | %6d | %10.4f | %10.4f | %10.4f | %21.1f |\n",
           impl, cap, items, t, avg, tmin, tmax, thr);
}



void print_footer() {
    printf("+----------+-----+---+---+--------+--------+------------+------------+------------+-----------------------+\n");
}
