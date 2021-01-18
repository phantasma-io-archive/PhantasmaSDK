import json
import requests
import sys

sys.path.append('../../Libs')
from VM import EventDecoder

def get_with_retry(url, method, params):
    RETRIES = 10
    url = url + "/" + method
    for i in range(10):
        try:
            data = requests.get(url=url, params=params, timeout=30)
            data = data.json()
            if data is None:
                raise Exception("No data found.")
            return data
        except Exception as ex:
            pass
        if i < RETRIES - 1:
            time.sleep(2 ** i)
        else:
            raise Exception("failed after all retries")


def parseEventTx(hash):
    PHAN_API_URL = "http://207.148.17.86:7078/api"
    params = dict(
        hashText=hash
    )
    tx = get_with_retry(PHAN_API_URL, "getTransaction", params)

    #print("RAW TX DATA", tx)
    if 'result' in tx:
        for event in tx['events']:
            if "Gas" not in event["kind"]:

                print("EVENT:", event["contract"], event["kind"], event["data"])

                dec = EventDecoder(event["data"])
                tokensymbol = dec.readString()
                nft_id_or_tokenamount = dec.readBigIntAccurate()
                chain = dec.readString()

                print("DECODED DATA:", tokensymbol, nft_id_or_tokenamount, chain, "\n")


if __name__ == '__main__':

    # EXAMPLE TX WITH FT
    print("############## FT TX ############## ")
    parseEventTx(
        "2C55319B7B8A57B31CA41EA8517E5A970A6C20885ED2B85053137B8393BB03AA")
    print("############## FT TX ############## ")

    # EXAMPLE TX WITH NFT
    print("############## FT TX ############## ")
    parseEventTx(
        "9DA4489D589E4B8FA3AB46FBA25ED0086EF4808DC4EC8C888FC3B4D3715BAE07")
    print("############## FT TX ############## ")
