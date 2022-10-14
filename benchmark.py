import asyncio
from aiohttp import ClientSession, ClientTimeout
async def fetch(url):
    timeout = ClientTimeout(total=5000)
    async with ClientSession() as session:
        async with session.get(url) as response:
            data = await response.read()
            print(data);
            return data

async def run(r):
    url = "http://localhost:5555"
    tasks = []

    # Fetch all responses within one Client session,
    # keep connection alive for all requests.
    for i in range(r):
        task = asyncio.ensure_future(fetch(url))
        tasks.append(task)

    responses = await asyncio.gather(*tasks, return_exceptions=True)
    # you now have all response bodies in this variable
    print(responses)

def print_responses(result):
    print(result)

loop = asyncio.get_event_loop()
future = asyncio.ensure_future(run(5000))
loop.run_until_complete(future)