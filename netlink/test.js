const { netlink } = just.library('./netlink.so', 'netlink')

just.print(netlink.calculate(1))
