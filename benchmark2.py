from gevent import monkey, spawn, joinall
monkey.patch_all()
import urllib3, certifi
from time import time
import gipc


worker = {}
num_threads = 10000

def fetch(num_threads, url, cpu):
    print('starting {}'.format(cpu))
    threads = []
    upool = urllib3.PoolManager(cert_reqs='CERT_REQUIRED', ca_certs=certifi.where(), num_pools=20, block=False)
    t0 = time()
    for i in range(num_threads):
        threads.append(spawn(upool.request, 'GET', url))
    x = joinall(threads)
    return x, time() - t0

def count_cpus():
    import multiprocessing
    cpus = multiprocessing.cpu_count()
    print(cpus)
    return cpus

def multicore(url):
    global worker
    with gipc.pipe() as (r,w):
        for cpu in range(count_cpus()):
            worker[str(cpu)] = gipc.start_process(target=fetch, args=(num_threads, url, cpu))
    for work in worker:
        worker[work].join()
    return worker

if __name__ == '__main__':
    multicore('http://localhost:5555')

    for work in worker:
        print( worker[work])